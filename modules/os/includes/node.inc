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
 * Usage

include_once drupal_get_path('module', 'os') . '/includes/node.inc';
$query = new OSNodeFieldQuery('announcement');
$query
  ->propertyOrderBy('created', 'DESC')
  ->fieldCondition('field_whatever', 'value', '1')
  ->range(0,3);
$result = $query->execute();

if (!isset($result['node'])) {
  // No results
}
else {
  $nodes = node_load_multiple(array_keys($result['node'));
}

 */