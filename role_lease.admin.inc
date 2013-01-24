<?php
/**
 * Generates the role lease editing form.
 */
function role_lease_form($form, &$form_state, $role_lease, $op = 'edit') {
  if ($op == 'clone') {
    $role_lease->label .= ' (cloned)';
    $role_lease->type = '';
  }

  $role_lease_user = $role_lease->user();

  //TODO: should not be able to change user!! Implement properly, lock all values!

  $form['uid'] = array(
    '#title' => t('User'),
    '#type' => 'textfield',
    '#autocomplete_path' => 'user/autocomplete',
    '#default_value' => isset($role_lease_user) ? $role_lease_user->name : NULL,
    '#description' => t('TODO.'),
    '#required' => TRUE,
  );

  if(isset($role_lease_user)) {
    $form['uid']['#attributes'] = array('readonly' => 'readonly');
    //will reflect in form state?
    $form['uid']['#value'] = $role_lease_user->name;
  }

  $form['type'] = array(
    '#title' => t('Type'),
    '#type' => 'select',
    '#options' => role_lease_type_options_list(),
    '#default_value' => $role_lease->type,
    '#description' => t('TODO.'),
    '#required' => TRUE,
  );

  /*
  $form['expires'] = array(
    '#title' => t('Expiry date'),
    '#type' => 'textfield', //TODO: change to date type, date module needed? 
    '#default_value' => $role_lease->expires,
    '#description' => t('Expiry date for the role lease.'),
    '#required' => TRUE,
  );
  */

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save role lease'), //TODO: add bundle label
    '#weight' => 40,
  );
  /*
  if (!$role_lease->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete role lease'), //TODO: see above
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('role_lease_form_submit_delete')
    );
  }
  */
  return $form;
}


function role_lease_form_validate($form, &$form_state) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'user')
    ->propertyCondition('name', $form_state['values']['uid']);
  $result = $query->execute();
  if(isset($result['user'])) {
    $uid = key($result['user']);
    $form_state['values']['uid'] = $uid;
  }
  else {
    form_error($form['uid'], t('Invalid user'));
  }
}


/**
 * Form API submit callback for the type form.
 */
function role_lease_form_submit(&$form, &$form_state) {
  $role_lease = entity_ui_form_submit_build_entity($form, $form_state);
  //dsm($role_lease);
  // Save and go back.
  $role_lease->acquire();
  $form_state['redirect'] = 'admin/structure/role_leases';
}

/**
 * Form API submit callback for the delete button.
 */
function role_lease_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/role_leases/manage/' . $form_state['role_lease']->type . '/delete';
}

?>
