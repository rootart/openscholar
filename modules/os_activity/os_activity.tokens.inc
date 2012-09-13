<?php

/**
 * @file
 * Tokens for the module OS Activity.
 */

/**
 * Implements hook_token_info().
 */
function os_activity_token_info() {
  $info['tokens']['node']['message_owner_name'] = array(
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

  // Only handeling comment and nodes.
  if ($type != 'message') {
    return;
  }

  foreach ($tokens as $name => $original) {
    if ($original != '[message:field-node-reference:message_owner_name]') {
      continue;
    }

    $replacements[$original] = os_activity_message_owner_name($data['message']);
  }

  return $replacements;
}
