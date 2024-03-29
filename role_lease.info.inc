<?php

/**
 * @file
 * Provides Entity metadata integration.
 */

/**
 * Extend the defaults.
 */
class RoleLeaseMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    /*
    $properties['label'] = array(
      'label' => t('Label'),
      'description' => t('The role lease label.'),
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer role leases',
      'schema field' => 'label',
    );
    */

    $properties['type'] = array(
      'type' => 'role_lease_type',
      'getter callback' => 'entity_property_getter_method',
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer role leases',
      'required' => TRUE,
      'description' => t('The role lease type.'),
    ) + $properties['type'];

    unset($properties['uid']);

    $properties['user'] = array(
      'label' => t('User'),
      'type' => 'user',
      'description' => t('The owner of the role lease.'),
      'getter callback' => 'entity_property_getter_method',
      'setter callback' => 'entity_property_setter_method',
      'setter permission' => 'administer role leases',
      'required' => TRUE,
      'schema field' => 'uid',
    );

    $properties['created'] = array(
      'label' => t('Date created'),
      'type' => 'date',
      'description' => t('The date the role lease was created.'),
      'setter callback' => 'entity_property_verbatim_set',
      'setter permission' => 'administer role leases',
      'schema field' => 'created',
    );

    $properties['changed'] = array(
      'label' => t('Date changed'),
      'type' => 'date',
      'schema field' => 'changed',
      'description' => t('The date the role lease was most recently updated.'),
    );

    return $info;
  }
}

/**
 * Implements hook_entity_property_info_alter().
 */
/*
function role_lease_entity_property_info_alter(&$info) {
  // Add related role leases to the user object.
  $properties = &$info['user']['properties'];
  foreach (role_lease_get_types() as $type_name => $type) {
    $properties['role lease_' . $type_name] = array(
      'type' => 'role_lease',
      'label' => t('@type_name role lease', array('@type_name' => drupal_ucfirst($type->label))),
      'description' => t("The users's @type_name role lease.", array('@type_name' => $type->label)),
      'getter callback' => 'role_lease_user_get_properties',
      'bundle' => $type_name,
    );
  }
}
*/
