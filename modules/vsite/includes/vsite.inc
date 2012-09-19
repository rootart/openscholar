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
 * Add the passed entity to a vsite
 *
 *  The entity that you wish to add to the vsite
 *  @param entity $entity
 *
 *  The type of entity you are passing (default node)
 *  @param string $entity_type
 *
 *  The vsite object to add the entity to
 *  @param space $vsite
 */
function vsite_add_entity($entity, $entity_type = 'node', $vsite = false){

  //Make sure the entity is valid
  list($id) = entity_extract_ids($entity_type, $entity);

  //Get the active space
  if(!$vsite){
    $vsite = spaces_get_space();
  }

  if($id && $vsite){

    $values = array(
        'entity_type' => $entity_type,
        'entity' => $entity,
    );

    //Add this entity to the active group
    $subscription = og_group($vsite->group_type, $vsite->id, $values);
    $gids[] = $subscription->gid;

    $wrapper = entity_metadata_wrapper($entity_type, $entity);
    $field = field_info_field($subscription->field_name);

    //If this was a member of other groups add them to the field
    if ($field['cardinality'] != 1) {
      foreach ($wrapper->{$subscription->field_name . '__og_membership'}->value() as $og_membership) {
        $gids[] = $og_membership->gid;
      }
    }

    //Add the new group to the entity
    $wrapper->{$subscription->field_name}->set(array_unique($gids));

    return $subscription;
  }

  return false;
}

/**
 * Remove the passed entity from a vsite
 *
 *  The entity that you wish to remove from a vsite
 *  @param entity $entity
 *
 *  The type of entity you are passing (default node)
 *  @param string $entity_type
 *
 *  The vsite object to add the entity to
 *  @param space $vsite
 */
function vsite_remove_entity($entity, $entity_type = 'node', $vsite = false){

  //Make sure the entity is valid
  list($id) = entity_extract_ids($entity_type, $entity);

  //Get the active space
  if(!$vsite){
    $vsite = spaces_get_space();
  }

  if($id && $vsite){

    //Remove this entity from the group
    return og_ungroup($vsite->group_type, $vsite->id, $entity_type, $id);
  }

  return false;
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