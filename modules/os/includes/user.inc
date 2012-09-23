<?php

/**
 * @file user.inc
 * Provides and CRUD functions needed for adding or removing users from your
 * openscholar site.
 *
 * Provides user management and crud functions
 *
 * os_user_create
 *  Creates a new drupal user and (optionally) automatically adds them to the
 *  current site.
 *
 * os_user_remove
 *  Removes a user from an Openscholar site (multi-tenant).
 *
 * os_role_grant
 *   Grant a user a role (standard or OG depending on multi-tenant).
 *
 * os_role_revoke
 *   Revoke a users role (standard or OG depending on multi-tenant).
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
 *   The new user account or false if there was an error.
 */
function os_user_create($user_info) {
  $account = user_save(NULL, $user_info);

  if (!$account || !isset($account->uid)){
    return FALSE;
  }

  if (module_exists('vsite')){
    module_load_include('inc', 'vsite', 'includes/vsite');
    return vsite_add_entity('user', $account);
  }

  return $account;
}

/**
 * Remove users from a OpenScholar site
 *
 * @param int $uid
 *  The identifier string for a context.
 *
 * @param $site_id
 *  Site ID.
 *
 * @param $group_type
 *  The group type you would like to add to.
 *
 * @return bool
 *  TRUE/FALSE
 */
/**
 * Add a user, and if you are in a group add them to your group.
 */
function os_user_remove($uid, $site_id = FALSE, $group_type = 'node') {
  $account = user_load($uid);

  // Do some basic validation.
  if (!$account) {
    // An invalid user has been passed.
    return FALSE;
  }

  if (!$site_id) {
    if (module_exists('vsite') && ($vsite = spaces_get_space())) {
      $site_id = $vsite->id;
    }
    else{
      // No SID passed and we are not in a site.
      return FALSE;
    }
  }

  og_ungroup($group_type, $site_id, 'user', $uid);

  return TRUE;
}
