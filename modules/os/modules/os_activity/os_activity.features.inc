<?php
/**
 * @file
 * os_activity.features.inc
 */

/**
 * Implements hook_views_api().
 */
function os_activity_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_default_message_type().
 */
function os_activity_default_message_type() {
  $items = array();
  $items['os_node'] = entity_import('message_type', '{
    "name" : "os_node",
    "description" : "OS node",
    "argument_keys" : [],
    "argument" : [],
    "category" : "message_type",
    "data" : { "purge" : { "override" : 0, "enabled" : 0, "quota" : "", "days" : "" } },
    "language" : "",
    "arguments" : null,
    "message_text" : { "und" : [
        {
          "value" : "@{message:field-node-reference:message_owner_name} has @action \\u003Ca href=\\u0027[message:field-node-reference:url]\\u0027\\u003E[message:field-node-reference:title]\\u003C\\/a\\u003E",
          "format" : "full_html",
          "safe_value" : "\\u003Cp\\u003E@{message:field-node-reference:message_owner_name} has @action \\u003Ca href=\\u0022[message:field-node-reference:url]\\u0022\\u003E[message:field-node-reference:title]\\u003C\\/a\\u003E\\u003C\\/p\\u003E\\n"
        }
      ]
    },
    "rdf_mapping" : []
  }');
  return $items;
}
