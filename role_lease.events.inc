<?php
function role_lease_role_lease_expire($entity) {
  rules_invoke_event('role_lease_expire', $entity);
}

function role_lease_role_lease_activate($entity) {
  rules_invoke_event('role_lease_activate', $entity);
}

function role_lease_role_lease_deactivate($entity) {
  rules_invoke_event('role_lease_deactivate', $entity);
}

?>
