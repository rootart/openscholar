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

    switch ($wrapper->getBundle()) {
      case 'personal':
        $replacements[$original] = $wrapper->author->label();
        break;

      case 'department':
      case 'project':
        $replacements[$original] = $wrapper->label();
        break;
    }
  }

  return $replacements;
}
