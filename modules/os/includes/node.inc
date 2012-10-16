<?php
// $Id$


/**
 * @file
 * Common functions for working with node content in OpenScholar.
 */

/**
 * Prepares an EntityFieldQuery with any necessary vsite-sensitive conditions.
 */
class OSNodeFieldQuery extends EntityFieldQuery {
  public function __construct($type = NULL) {
    // Limits only to published nodes.
    $this
      ->entityCondition('entity_type', 'node')
      ->propertyCondition('status', 1);

    if (isset($type) && strlen($type) > 0) {
      $this->propertyCondition('type', $type);
    }

    if (!module_exists('vsite')) {
      return;
    }

    $space = spaces_get_space();
    if (!is_object($space) || !is_numeric($space->id)) {
      return;
    }

    $this
      ->fieldCondition(OG_AUDIENCE_FIELD, 'target_id', $space->id);
  }
}

/**
 * Example usage of class OSNodeFieldQuery.
 */
function _os_example_get_announement_nodes() {
  // Includes this file -- for use in other modules.
  //include_once drupal_get_path('module', 'os') . '/includes/node.inc';

  // Creates a new EntityFieldQuery for nodes of the given type, accounting for
  // current space if the vsite module is enabled.
  $query = new OSNodeFieldQuery('announcement');
  $query
    ->propertyOrderBy('created', 'DESC')
    ->fieldCondition('field_whatever', 'value', '1')
    ->range(0,3);
  $result = $query->execute();

  // Handles results.
  if (!isset($result['node'])) {
    // No results
    return array();
  }
  else {
    $nodes = node_load_multiple(array_keys($result['node']));
    return $nodes;
  }
}