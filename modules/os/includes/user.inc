<?php

/**
 * Provides and CRUD functions needed for adding or removing users from your openscholar site
 *
 * @file user.inc
 *
 * Provides user management and crud functions
 *
 * os_user_create
 *   Creates a new drupal user and (optionally) automatically adds them to the current site
 *
 * os_user_remove
 *  Removes a user from an Openscholar site (multi-tenant)
 *
 * os_role_grant
 *   Grant a user a role (standard or OG depending on multi-tenant)
 *
 * os_role_revoke
 *   Revoke a users role (standard or OG depending on multi-tenant)
 *
 */

/**
 * Creates a new drupal user
 *
 * @param $user_info
 *   An array of fields and values to save. For example array('name'
 *   => 'My name'). Key / value pairs added to the $edit['data'] will be
 *   serialized and saved in the {users.data} column.
 *
 * @return $account
 *   The new user account or false if there was an error
 */
function os_user_create($user_info){

  $account = user_save(null,$user_info);

  if(!$account || !isset($account->uid)){
    return false;
  }

  if (module_exists('vsite')){
    module_load_include('inc', 'vsite', 'includes/vsite');
    return vsite_add_entity('user', $account);
  }

  return $account;
}

/**
 * Grant a role to a user.
 *
 * @param $uid
 *   The user ID.
 * @param $rid
 *   The role ID.
 * @param $site_id
 *   Group ID (OPTIONAL multi-tenant only)
 */
function os_role_grant($uid, $role_name, $site_id = FALSE) {
  if (module_exists('vsite') && $site_id) {
    // Add role via OG.
    foreach (og_roles('node', 'vsite', $site_id) as $rid => $role) {
      if ($role == $role_name){
        og_role_grant('node', $site_id, $uid, $rid);
        return TRUE;
      }
    }
  }
  elseif ($role = user_role_load_by_name($role_name)) {
    // Add role via standard Drupal.
    $user = user_load($uid,TRUE);
    $user->role[$role->rid] = $role;
    user_save($user);
    return TRUE;
  }


  // Invalid Role.
  return FALSE;
}

/**
 * Revoke a role from a user.
 *
 * @param $uid
 *   The user ID.
 * @param $role_name
 *   The role name.
 * @param $site_id
 *   Group ID (OPTIONAL multi-tenant only)
 */
function os_role_revoke($uid, $role_name, $site_id = FALSE) {
  if (module_exists('vsite') && $site_id) {
    // Add role via OG.
    foreach (og_roles('node', 'vsite', $site_id) as $rid => $role) {
      if ($role == $role_name){
        og_role_revoke('node', $site_id, $uid, $rid);
        return TRUE;
      }
    }
  }
  elseif (($role = user_role_load_by_name($role_name)) && isset($user->role[$role->rid])) {
    // Add role via standard Drupal.
    $user = user_load($uid,TRUE);
    unset($user->role[$role->rid]);
    user_save($user);
    return TRUE;
  }

  // Invalid role.
  return FALSE;
}

/**
 * Remove users from a OpenScholar site
 *
 * @param int $uid
 * The identifier string for a context
 *
 * @param $site_id
 * Site ID
 *
 * @param $group_type
 * The group type you would like to add to
 *
 * @return bool
 * Success
 */
/**
 * Add a user, and if you are in a group add them to your group
 */
function os_user_remove($uid, $site_id = false, $group_type = 'node'){
  $account = user_load($uid);

  //Do some basic validation
  if (!$account) {
    //An invalid user has been passed
    return false;
  }

  if(!$site_id){
    if(module_exists('vsite') && ($vsite = spaces_get_space())){
      $site_id = $vsite->id;
    }else{
      //No SID passed and we are not in a site
      return false;
    }
  }

  og_ungroup($group_type, $site_id, 'user', $uid);

  return true;
}
