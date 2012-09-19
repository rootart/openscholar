<?php

/**
 * @file
 * Tokens integration for the module OS Activity.
 */

/**
 * Implements hook_token_info().
 */
function os_activity_token_info() {
  $info['tokens']['node']['message-owner'] = array(
    'name' => t('Message owner name'),
    'description' => t('Display the message owner name acording the the virtual site settings.'),
  );

  return $info;
}

/**
 * Implements hook_tokens().
 */
function os_activity_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  // Only handeling messages.
  if ($type != 'node') {
    return;
  }

  foreach ($tokens as $name => $original) {
    if ($name != 'message-owner') {
      continue;
    }

    $wrapper = entity_metadata_wrapper('node', $data['node']);

    // The virtual site owner can decide of he want to present the username or the
    // virtual site name as the message owner.
    $replacements[$original] = $wrapper->value() == 'username' ? $wrapper->author->label() : $wrapper->label();
  }

  return $replacements;
}
