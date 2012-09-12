<?php
/**
 * Provides and CRUD functions needed for adding or removing users from a vsite
 *
 * @file user.inc
 *
 * Provides user management and crud functions
 *
 * vsite_user_add
 *  Add users to a OpenScholar vsite
 *
 * vsite_user_remove
 *  Remove a user from a OpenScholar vsite
 *
 * vsite_role_grant
 *   Grant a user a role
 *
 * vsite_role_revoke
 *   Revoke a user a role
 */


/**
 * Add users to a OpenScholar vsite.
 *
 * @param $account
 *   The user ID or object.
 * @param $site_id
 *   Site ID.
 *
 * @return
 *   Ther return value of vsite_add_entity().
 */
function vsite_user_add($account, $site_id = FALSE){
  if (!$site_id && !$og_conext = og_context()) {
    return;
  }

  $account = is_object($account) ? $account : user_load($account);
  if (!$account) {
    return;
  }

  module_load_include('inc', 'vsite', 'includes/vsite');

  //Add the user to this site.
  return vsite_add_entity('user', $account, $site_id);
}

/**
 * Remove a user from a OpenScholar vsite.
 *
 * @param $account
 *   The user ID or object.
 * @param $site_id
 *   Site ID.
 */
function vsite_user_remove($account, $site_id = FALSE){
  if (!$site_id && !$og_conext = og_context()) {
    return;
  }

  $account = is_object($account) ? $account : user_load($account);
  if (!$account) {
    return;
  }

  module_load_include('inc', 'vsite', 'includes/vsite');

  // Remove the user from this site.
  return vsite_remove_entity('user', $account, $site_id);
}

/**
 * Grant a role to a user within a vsite.
 *
 * @param $uid
 *   The user ID.
 * @param $rid
 *   The role ID.
 * @param $site_id
 *   The Group ID
 * @param $group_type
 *   The Group Type
 */
function vsite_role_grant($uid, $role_name, $site_id = false, $group_type = 'node') {

  if(!$site_id){
    $vsite = spaces_get_space();
    if($vsite){
      $site_id = $vsite->id;
      $group_type = $vsite->group_type;
    }
  }

  if($site_id){
    //Add role via OG
    $roles = og_roles($group_type, 'vsite', $site_id);
    foreach ($roles as $rid => $role) {
      if($role == $role_name){
        og_role_grant($group_type, $site_id, $uid, $rid);
        return true;
      }
    }
  }

  //Unable to determine group
  return false;
}

/**
 * Revoke a role to a user within a vsite.
 *
 * @param $uid
 *   The user ID.
 * @param $rid
 *   The role ID.
 * @param $site_id
 *   The Group ID
 * @param $group_type
 *   The Group Type
 */
function vsite_role_revoke($uid, $role_name, $site_id = false, $group_type = 'node') {

  if(!$site_id){
    $vsite = spaces_get_space();
    if($vsite){
      $site_id = $vsite->id;
      $group_type = $vsite->group_type;
    }
  }

  if($site_id){
    //Add role via OG
    $roles = og_roles($group_type, 'vsite', $site_id);
    foreach ($roles as $rid => $role) {
      if($role == $role_name){
        og_role_revoke($group_type, $site_id, $uid, $rid);
        return true;
      }
    }
  }

  //Unable to determine group
  return false;
}
