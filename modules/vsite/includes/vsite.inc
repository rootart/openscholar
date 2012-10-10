<?php

/**
 * @file
 * Provide API function to add and remove entities from virtual-sites.
 */

/**
 * Add the passed entity to a vsite.
 *
 * The entity that you wish to add to the vsite.
 * @param $entity
 *   The entity object.
 * @param $entity_type
 *   Optional; The type of entity you are passing. Defaults to "node".
 * @param space $vsite
 *   Optional; The vsite object to add the entity to. Defaults to the
 *   active space.
 *
 * @return
 *   The newly created OG-membership or NULL if not valid.
 */
function vsite_add_entity($entity, $entity_type = 'node', $vsite = FALSE){

  //Make sure the entity is valid
  list($id) = entity_extract_ids($entity_type, $entity);
  if (!$id) {
    return;
  }

  // Get the active space.
  $vsite = $vsite ? $vsite : spaces_get_space();
  if (!$vsite) {
    return;
  }

  $values = array(
    'entity_type' => $entity_type,
    'entity' => $entity,
  );

  //Add this entity to the active group.
  $og_membership = og_group($vsite->group_type, $vsite->id, $values);

  // Tell OG to ignore the existing field settings on this entity.
  $entity->skip_og_membership = TRUE;

  return $og_membership;
}

/**
 * Remove the passed entity from a vsite
 *
 * The entity that you wish to add to the vsite.
 * @param $entity
 *   The entity object.
 * @param $entity_type
 *   Optional; The type of entity you are passing. Defaults to "node".
 * @param space $vsite
 *   Optional; The vsite object to add the entity to. Defaults to the
 *   active space.
 */
function vsite_remove_entity($entity, $entity_type = 'node', $vsite = FALSE){
  // Make sure the entity is valid.
  list($id) = entity_extract_ids($entity_type, $entity);
  if (!$id) {
    return;
  }

  // Get the active space.
  $vsite = $vsite ? $vsite : spaces_get_space();
  if (!$vsite) {
    return;
  }

  // Remove this entity from the group.
  og_ungroup($vsite->group_type, $vsite->id, $entity_type, $id);
}

/**
 * Creates a new vsite
 *
 * @param $name
 * 	 Name of site.
 * @param $domain
 * 	 Path for new site.
 * @param $uid
 * 	The user ID to assocaite with the new VSite.
 * @param $preset
 * 	Type of vsite to create.
 *
 * @return
 *   the SPACE object if created or FALSE.
 */
function vsite_create_vsite($name, $domain, $uid, $preset = FALSE) {
  $node = new stdClass();
  $node->type = 'vsite';
  node_object_prepare($node);

  $node->title = $name;
  $node->body = array(LANGUAGE_NONE => array(array('value' => '')));
  $node->purl = array(
    'value' => $domain,
    'provider'=>'vsite_og',
    'id'=>NULL,
  );
  $node->uid = $uid;
  $node->spaces_preset_og = $preset;
  $node->language = LANGUAGE_NONE;
  node_save($node);

  if ($vsite = spaces_load('og', $node->nid)) {
    return $vsite;
  }

  watchdog('vsite', t('Error creating vsite: @nid', array('@nid'=>$node->nid)), array(), WATCHDOG_ERROR);
}

/**
 * Gets all vsites a user "owns".
 *
 * @param $uid
 *   The user ID of the user to lookup. Defaults to current user.
 *
 * @return
 *   Array with VSite objects if found, or empty array.
 */
function vsite_get_vsite_by_owner($uid = NULL) {
  if (!$uid) {
    global $user;
    $uid = $user->uid;
  }

  $group_bundles = og_get_all_group_bundle();
  if (empty($group_bundles['node'])){
    //can't determine group content types
    return array();
  }
  $bundles = array_keys($group_bundles['node']);

  $query = new EntityFieldQuery();
  $result = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('uid', $uid)
    ->entityCondition('bundle', $bundles, 'IN')
    ->execute();

  if (empty($result['node'])) {
    return array();
  }

  foreach(array_keys($result['node']) as $id) {
    $vsites[] = spaces_load('og', $id);
  }

  // Filter out any spaces that were not loaded.
  return array_filter($vsites);
}

/**
 * Gets all vsites a user is a member of
 *
 * @param $uid
 *   The user ID of the user to lookup. Defaults to current user.
 *
 * @return
 *   Array with VSite objects if found, or NULL.
 */
function vsite_get_user_vsites($uid = NULL) {

  //Retrieve all group memberships for the user
  $groups = og_get_entity_groups('user', $uid);

  if (empty($groups['node'])) {
    return array();
  }

  $vsites = array();
  foreach($groups['node'] as $mid => $gid) {
    $vsites[]  = spaces_load('og', $gid);
  }

  // Filter out any spaces that were not loaded.
  return array_filter($vsites);
}
