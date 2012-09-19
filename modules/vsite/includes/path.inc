<?php

/**
 * Return the pathauto settings modified to work inside vsites
 */
function _vsite_get_pathauto_settings(){
  $pathauto_settings = array();

  $bundles = array_keys(os_get_bundles(true));
  $entity = 'node';
  foreach ($bundles as $bundle) {
    $pattern = pathauto_pattern_load_by_entity($entity,$bundle);
    $pathauto_settings["pathauto_{$entity}_{$bundle}_pattern"] = "[vsite:site-purl]/".$pattern;
  }

  return $pathauto_settings;
}

/**
 * Integrates pathauto & purl
 *
 * - Adds validation to verify paths without a purl prefix do not already exist
 * - Adds purl prefix to manually created paths to preserve namespace
 *
 * @see vsite_node_presave()
 */
function _vsite_alter_node_path_settings(&$node) {
  $vsite = vsite_get_vsite();
  if (!(module_exists('path') && module_exists('pathauto') && $vsite)) {
    return;
  }

  module_load_include('inc', 'pathauto');
  $pattern = _vsite_get_pathauto_pattern($node);

  // For pathauto generated aliases, verify no naming collisions
  // Only checks aliases if a pathauto pattern exists and pathauto will alias.
  if ($pattern && (!isset($node->pathauto_perform_alias) || $node->pathauto_perform_alias)) {
    // Generates the pathauto alias and removes pURL modifiers
    $alias = _vsite_alias_without_purl($node);

    // Aborts pathauto for this node if the path already exists.
    if (_pathauto_path_is_callback($alias)) {
      $node->pathauto_perform_alias = FALSE;
      return;
    }
  }

  // For manually entered aliases, prefix alias with the purl modifier.
  // Note that Drupal has already validated the path prior to hook_node_presave,
  // so there's no way that an existing path has gotten to this point.
  else {
    // Did they provide a path? Is the site purl already prepended?
    if (strlen($node->path) && strpos($node->path, $vsite->group->purl . "/") !== 0 && $node->nid != $vsite->group->nid && strlen($vsite->group->purl)) {
      $node->path = $vsite->group->purl . "/" . $node->path;
    }
  }
}

/**
 * Returns the pathauto alias for a node, with any pURL modifiers removed.
 *
 * @param object $node
 *
 * @return string $alias
 *
 * @see _vsite_alter_node_path_settings()
 */
function _vsite_alias_without_purl($node) {
  $uri = entity_uri('node', $node);
  $language = !empty($node->language) ? $node->language : LANGUAGE_NONE;
  $alias = pathauto_create_alias('node', 'return', $uri['path'], array('node' => $node), $node->type, $language);

  // Removes purl elements from alias string.
  $processor = purl_get_processor(PURL_VSITE_PATH);
  $elements = purl_parse($processor, $alias);
  foreach ($elements as $element) {
    $alias = $processor->remove($alias, $element);
  }

  return $alias;
}

/**
 * Gets the most specific pattern for a node, or FALSE if no default defined.
 */
function _vsite_get_pathauto_pattern($node) {
  if (variable_get('language_content_type_' . $node->type, 0)) {
    // @todo make sure language default var name same
    $pattern = trim(variable_get('pathauto_node_' . $node->type . '_' . $node->language . '_pattern', FALSE));
  }
  // Otherwise, gets the default pattern
  if (empty($pattern)) {
    $pattern = trim(variable_get('pathauto_node_' . $node->type . '_pattern', FALSE));
    if (empty($pattern)) {
      $pattern = trim(variable_get('pathauto_node_pattern', FALSE));
    }
  }
  return $pattern;
}