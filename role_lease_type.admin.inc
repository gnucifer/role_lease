<?php
/**
 * UI controller.
 */
class RoleLeaseTypeUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage role lease types.';
    return $items;
  }
}

/**
 * Generates the role lease type editing form.
 */
function role_lease_type_form($form, &$form_state, $role_lease_type, $op = 'edit') {

  if ($op == 'clone') {
    $role_lease_type->label .= ' (cloned)';
    $role_lease_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $role_lease_type->label,
    '#description' => t('The human-readable name of this role lease type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($role_lease_type->type) ? $role_lease_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $op != 'clone',
    '#machine_name' => array(
      'exists' => 'role_lease_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this role lease type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['lease_time'] = array(
    '#title' => t('Lease time'),
    '#type' => 'textfield',
    '#default_value' => $role_lease_type->lease_time,
    '#description' => t('The role lease time duration'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['lease_time_unit'] = array(
    '#title' => t('Lease time unit'),
    '#type' => 'select',
    '#options' => role_lease_duration_options(),
    '#default_value' => $role_lease_type->lease_time_unit,
    '#description' => t('The role lease time unit'),
    '#required' => TRUE,
  );

  $form['weight'] = array(
    '#title' => t('Weight'),
    '#type' => 'weight',
    '#default_value' => $role_lease_type->weight,
    '#description' => t('TODO'),
    '#required' => TRUE,
  );

  $rid_options = user_roles(TRUE);
  unset($rid_options[2]); //We don't care about "authenticated user"

  $form['rid'] = array(
    '#title' => t('Role'),
    '#type' => 'select',
    '#options' => $rid_options,
    '#default_value' => $role_lease_type->rid,
    '#description' => t('Role to lease'),
    '#required' => TRUE
  );

  $form['data']['#tree'] = TRUE;
  $form['data']['registration'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show during user account registration.'),
    '#default_value' => !empty($role_lease_type->data['registration']),
  );

  $trigger_event = isset($role_lease_type->data['before_expire_event_trigger']) ? $role_lease_type->data['before_expire_event_trigger'] : FALSE;
  $form['data']['before_expire_event_trigger'] = array(
    '#type' => 'checkbox',
    '#title' => t('Trigger event before expire'),
    '#description' => 'Trigger and configure the "Configurable time before role lease expire" rules event', 
    '#default_value' => $trigger_event,
  );

  //TODO: if module exists?
  $form['data']['before_expire_event'] = array(
    '#title' => t('Before expire event'),
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        '#edit-data-before-expire-event-trigger' => array('checked' => TRUE),
      ),
    ),
    '#description' => 'Set a duration that will determine when the "Configurable time before role lease expire" rules event is triggered',
  );

  $form['data']['before_expire_event']['time'] = array(
    '#title' => t('Time'),
    '#type' => 'textfield',
    '#default_value' => isset($role_lease_type->data['before_expire_event']) ? $role_lease_type->data['before_expire_event']['time'] : 0,
    '#description' => t('The time to trigger event before expire.'),
    //'#required' => $trigger_event,
    '#size' => 30,
  );
  $form['data']['before_expire_event']['time_unit'] = array(
    '#title' => t('Time unit'),
    '#type' => 'select',
    '#options' => role_lease_duration_options(),
    '#default_value' => isset($role_lease_type->data['before_expire_event']) ? $role_lease_type->data['before_expire_event']['time_unit'] : NULL,
    '#description' => t('The time to trigger event before expire time unit.'),
    //'#required' => $trigger_event,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save role lease type'),
    '#weight' => 40,
  );

  if (!$role_lease_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete role lease type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('role_lease_type_form_submit_delete')
    );
  }
  
  field_attach_form('role_lease_type', $role_lease_type, $form, $form_state); //TODO: handle language and translations? Probaby, on field level, perhaps check in entity translations

  return $form;
}

function role_lease_type_form_validate(&$form, &$form_state) {
  $values = $form_state['values'];

  if($values['data']['before_expire_event_trigger']) {
    $event_values = $values['data']['before_expire_event'];
    //Only date DateInterval with a fixed start and end date will yield an actual amount of time (we settle for days in our case)
    $event_interval = role_lease_date_interval($event_values['time'], $event_values['time_unit']);
    $event_interval_now = role_lease_date_now_diff($event_interval);
    $event_days = intval($event_interval_now->format('%a'));
    
    $lease_time_interval = role_lease_date_interval($values['lease_time'], $values['lease_time_unit']);
    $lease_time_interval_now = role_lease_date_now_diff($lease_time_interval);
    $lease_time_days = intval($lease_time_interval_now->format('%a'));

    if($event_days >= $lease_time_days) {
      //No point in trying to make any exact predictions about validity since number of days in months can vary etc
      //We assume the user know what he/she is doing and just issue a warning if we find the interval seems invalid
      drupal_set_message(
        t(
          'You have set the offset for the "before expire" event to a larger time period than the lease time for !role_lease_type. This might cause strange behavior depending on your setup.',
          array('@role_lease_type' => $form_state['role_lease_type']->label)
        ),
        'warning'
      );
    }
  }
}



/**
 * Form API submit callback for the type form.
 */
function role_lease_type_form_submit(&$form, &$form_state) {
  $role_lease_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  $role_lease_type->save();
  $form_state['redirect'] = 'admin/structure/role_lease_types';
}

/**
 * Form API submit callback for the delete button.
 */
function role_lease_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/role_lease_types/manage/' . $form_state['role_lease_type']->type . '/delete';
}
