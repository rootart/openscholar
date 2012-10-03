<?php


/**
 * @file
 * Hooks provided by the Organic groups vocabulary module.
 */

/**
 * @addtogroup hooks
 * @{
 */


/**
 * Allow modules to return TRUE if we are insiede an OG vocab admin
 * context.
 */
function hook_og_vocab_is_admin_context() {
  $item = menu_get_item();
  return $item['path'] == 'foo/admin';
}

/**
 * @} End of "addtogroup hooks".
 */
