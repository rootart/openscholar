<?php
/**
 * Provides and CRUD functions relating to the vsite group
 *
 * @file vsite.inc
 *
 * Provides vsite management and crud functions
 *
 * vsite_add_entity
 *   Generic function to add a passed entity to a vsite
 *
 * vsite_create_vsite
 *   Creates a new vsite group
 *
 * vsite_get_vsite_by_owner
 *   Gets all of the groups owned by a user
 *
 */


/**
 * Add the passed entity to a vsite.
 *
 * @param $entity_type
 *   The entity type. Defaults to node.
 * @param $entity
 *   The entity that you wish to add to the vsite
 * @param $vsite_id
 *   The site ID (i.e. group ID).
 *
 * @return
 *   An OgMembership entity if created, or NULL.
 */
function vsite_add_entity($entity_type , $entity, $vsite_id = FALSE) {
  list($id) = entity_extract_ids($entity_type, $entity);
  if (!$id) {
    // Entity is not valid.
    return;
  }

  if (!$vsite_id && !$og_context = og_context()) {
    // Can't get a group context.
    return;
  }

  $values = array(
    'entity_type' => $entity_type,
    'entity' => $entity,
  );

  return og_group($og_context['group_type'], $og_context['gid'], $values);
}

/**
 * Remove the passed entity from a vsite.
 *
 * @param $entity_type
 *   The entity type. Defaults to node.
 * @param $entity
 *   The entity that you wish to add to the vsite
 * @param $vsite_id
 *   The site ID (i.e. group ID).
 */
function vsite_remove_entity($entity_type , $entity, $vsite_id = FALSE) {
  list($id) = entity_extract_ids($entity_type, $entity);
  if (!$id) {
    // Entity is not valid.
    return;
  }

  if (!$vsite_id && !$og_context = og_context()) {
    // Can't get a group context.
    return;
  }

  og_ungroup($og_context['group_type'], $og_context['gid'], $entity_type, $id);
}

/**
 * @function vsite_create_vsite
 *
 * Creates a new vsite
 *
 *
 * @param string $name
 * 	Name of site
 * @param string $domain
 * 	Path for new site
 * @param string $uid
 * 	Owner
 * @param string $preset
 * 	Type of vsite to create.
 *
 * @return vsite $vsite
 */
function vsite_create_vsite($name, $domain, $uid, $preset = FALSE) {
  $node = new stdClass();
  $node->type = 'vsite';
  node_object_prepare($node);

  $node->title = $name;
  $node->body = array('und' => array(array('value' => '')));
  $node->purl = array('value' => $domain, 'provider'=>'vsite_og', 'id'=>NULL);
  $node->uid = $uid;
  $node->spaces_preset_og = $preset;
  node_save($node);

  if ($vsite = spaces_load('og', $node->nid)) {
    return $vsite;
  } else {
    error_log(t('Error creating vsite: @nid', array('@nid'=>$node->nid)), LOG_WARNING);
    return FALSE;
  }
}

/**
 * Gets all vsites a user "owns"
 *
 * The UID of the user to lookup
 * Defaults to current user
 * @param $uid
 **/
function vsite_get_vsite_by_owner($uid = NULL) {

  if (!$uid) {
    global $user;
    $uid = $user->uid;
  }

  $group_bundles = og_get_all_group_bundle();
  if(empty($group_bundles['node'])){
    //can't determine group content types
    return false;
  }
  $bundles = array_keys($group_bundles['node']);

  $query = new EntityFieldQuery();
  $result = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('uid', (int) $uid)
    ->entityCondition('bundle', $bundles, 'IN')
    ->execute();

  $vsites = array();
  if (isset($result['node'])) {
    foreach(array_keys($result['node']) as $id) {
      $vsites[] = spaces_load('og', $id);
    }
  }

  return array_filter($vsites);
}

/**
 * Gets all vsites a user is a member of
 *
 * The UID of the user to lookup
 * Defaults to current user
 * @param $uid
 **/
function vsite_get_user_vsites($uid = NULL) {

  //Retrieve all group memberships for the user
  $groups = og_get_entity_groups('user',$uid);

  $vsites = array();
  if (isset($groups['node'])) {
    foreach($groups['node'] as $mid => $gid) {
      //Load the space object
      $vsites[]  = spaces_load('og', $gid);
    }
  }

  //Filter out any spaces that were not loaded
  return array_filter($vsites);
}
