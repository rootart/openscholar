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

  $info['tokens']['node']['group-site-logo'] = array(
    'name' => t('Group site logo'),
    'description' => t("Display the group's site logo."),
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
    if ($name == 'message-owner') {
      $wrapper = entity_metadata_wrapper('node', $data['node']);

      // The virtual site owner can decide of he want to present the username or
      // the virtual site name as the message owner.
      $replacements[$original] = $wrapper->field_message_display_name->value() == 'username' ? $wrapper->author->label() : $wrapper->label();
    }
    else if ($name == 'group-site-logo') {
      // A better token for the site logo. The original token is a dumb one:
      // when the field is empty, a missing/broken image appears. This token
      // takes care of that and displays nothing when the field is empty.
      $group_node = entity_metadata_wrapper('node', $data['node'])->og_group_ref->get(0)->value();

      if (empty($group_node->field_site_logo)) {
        // Default image when there is no site logo.
        $replacements[$original] = theme('image', array(
          'path' => drupal_get_path('module', 'os_activity') . '/images/mini_logo.jpg',
        ));
      }
      else {
        $replacements[$original] = theme('image_style', array(
          'path' => $group_node->field_site_logo[LANGUAGE_NONE][0]['uri'],
          'style_name' => 'thumbnail',
        ));
      }
    }
  }

  return $replacements;
}
