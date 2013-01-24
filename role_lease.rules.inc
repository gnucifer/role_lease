<?php 
/**
 * @file rules integration
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_file_info()
 */
function role_lease_rules_file_info() {
  return array('role_lease.eval');
}

/**
 * Role lease rules integration access callback.
 */
function role_lease_rules_integration_access($type, $name) {
  if ($type == 'event' || $type == 'condition') {
    return entity_access('view', 'role_lease');
  }
}

/**
 * Implements hook_rules_event_info()
 */
function role_lease_rules_event_info() {
  $items = array(
    'role_lease_before_expire' => array(
      'label' => t('Configurable time before a role lease expires'),
      'group' => t('Role lease'),
      'variables' => array(
        'role_lease' => array(
          'type' => 'role_lease', //ENTITY???
          'label' => t('Role lease'),
        ),
        'role_lease_type' => array(
          'type' => 'role_lease_type',
          'label' => t('Role lease type'),
        ),
      ),
      'access callback' => 'role_lease_rules_integration_access',
    ),
    'role_lease_expire' => array(
      'label' => t('Before role lease expires'),
      'group' => t('Role lease'),
      'variables' => array(
        'role_lease' => array(
          'type' => 'role_lease',
          'label' => t('Role lease'),
        ),
        'role_lease_type' => array(
          'type' => 'role_lease_type',
          'label' => t('Role lease type'),
        ),
      ),
      'access callback' => 'role_lease_rules_integration_access',
    ),
    'role_lease_acquire' => array(
      'label' => t('Before role lease is acquired'),
      'group' => t('Role lease'),
      'variables' => array(
        'role_lease' => array(
          'type' => 'role_lease',
          'label' => t('Role lease'),
        ),
        'role_lease_type' => array(
          'type' => 'role_lease_type',
          'label' => t('Role lease type'),
        ),
      ),
      'access callback' => 'role_lease_rules_integration_access',
    ),
    'role_lease_release' => array(
      'label' => t('Before role lease is released'),
      'group' => t('Role lease'),
      'variables' => array(
        'role_lease' => array(
          'type' => 'role_lease',
          'label' => t('Role lease'),
        ),
        'role_lease_type' => array(
          'type' => 'role_lease_type',
          'label' => t('Role lease type'),
        ),
      ),
      'access callback' => 'role_lease_rules_integration_access',
    ),
  );
  // Specify that on presave the node is saved anyway.
  // $items['node_presave']['variables']['node']['skip save'] = TRUE;
  return $items;
}

/**
 * Returns some parameter suitable for using it with a node
 */
/*
function rules_events_node_variables($node_label, $update = FALSE) {
  $args = array(
    'node' => array('type' => 'node', 'label' => $node_label),
  );
  if ($update) {
    $args += array(
      'node_unchanged' => array(
        'type' => 'node',
        'label' => t('unchanged content'),
        'handler' => 'rules_events_entity_unchanged',
      ),
    );
  }
  return $args;
}
 */

/**
 * Implements hook_rules_condition_info() on behalf of the node module.
 */
function role_lease_rules_condition_info() {
  $defaults = array(
    'parameter' => array(
      'role_lease' => array('type' => 'role_lease', 'label' => t('Role lease')),
    ),
    'group' => t('Role lease'),
    'access callback' => 'role_lease_rules_integration_access',
  );
  $items['role_lease_is_of_type'] = $defaults + array(
    'label' => t('Role lease is of type'),
    'help' => t('Evaluates to TRUE if the given role lease is of one of the selected role lease types.'),
    'base' => 'role_lease_rules_condition_role_lease_is_of_type', //TODO
  );
  $items['role_lease_is_of_type']['parameter']['type'] = array(
    'type' => 'list<text>',
    'label' => t('Role lease'),
    'options list' => 'role_lease_type_options_list', //TODO
    'description' => t('The role lease type(s) to check for.'),
    'restriction' => 'input',
  );
  $items['role_lease_is_active'] = $defaults + array(
    'label' => t('Role lease is active'),
    'base' => 'role_lease_rules_condition_role_lease_is_active', //TODO
  );
  $items['role_lease_is_expired'] = $defaults + array(
    'label' => t('Role lease is expired'),
    'base' => 'role_lease_rules_condition_role_lease_is_expired', //TODO
  );
  return $items;
}

/**
 * Implements hook_rules_action_info()
 */
function role_lease_rules_action_info() {
  $defaults = array(
    'parameter' => array(
      'role_lease' => array('type' => 'role_lease', 'label' => t('Role lease'), 'save' => TRUE),
    ),
    'group' => t('Role lease'),
    'access callback' => 'role_lease_rules_admin_access', //TODO
  );
  // Add support for declared core actions.
  $core_actions = role_lease_action_info();
  $items = array();
  foreach ($core_actions as $base => $core_action_info) {
    $action_name = str_replace('_action', '', $base);
    $items[$action_name] = $defaults + array(
      'label' => $core_action_info['label'],
      'base' => $base,
    );
  }
  return $items;
}

/**
 * Nules integration admin access callback.
 */
function role_lease_rules_admin_access() {
  return user_access('administer role leases');
}
