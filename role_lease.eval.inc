<?php

/**
 * @file
 * Contains rules integration for role lease needed during evaluation.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Condition: Check for selected content types
 */
function role_lease_rules_condition_role_lease_is_of_type($role_lease, $types) {
  return in_array($role_lease->type, $types);
}

/**
 * Condition: Check if the node is published
 */
function role_lease_rules_condition_role_lease_is_active($role_lease) {
  return (bool)$node->role_lease;
}

/**
 * Condition: Check if the node is expired
 */
function role_lease_rules_condition_role_lease_is_expired($role_lease) {
  return $node->expiry_time < time();
}

?>
