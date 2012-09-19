<?php

/**
 * Expose openscholar blocks as context reactions.
 */
class os_context_reaction_block extends context_reaction_block {

  /**
   * An alternative version of block_list() that provides any context enabled
   * blocks.
   */
  function block_list($region) {
    module_load_include('module', 'block', 'block');
    ctools_include('layout','os');

    $context_blocks = &drupal_static('context_reaction_block_list');;
    $contexts = context_active_contexts();

    if (!isset($context_blocks)) {
      $info = $this->get_blocks();
      $context_blocks = array();
      $all_blocks = array();
      $context_order = array_keys($contexts);
      usort($context_order, '_os_context_sort_by_hierarchy');

      foreach ($context_order as $key) {
        $context = $contexts[$key];
        $options = $this->fetch_from_context($context);
        if (!empty($options['blocks'])) {
          foreach ($options['blocks'] as $block) {
            $block = (object) $block;
            $block->context = $context->name;
            $block->bid = "{$block->module}-{$block->delta}";
            $block->title = isset($info[$block->bid]->title) ? $info[$block->bid]->title : NULL;
            $block->cache = isset($info[$block->bid]->cache) ? $info[$block->bid]->cache : DRUPAL_NO_CACHE;
            // Let child contexts override parents.
            $all_blocks[$block->bid] = $block;
          }
        }
      }

      // Put blocks into regions.
      foreach ($all_blocks as $block) {
      	$context_blocks[$block->region][$block->bid] = $block;
      }

      $this->is_editable_check($context_blocks);
      foreach ($context_blocks as $r => $blocks) {
        // do nothing for blocks that are in no region
        if (!$r) continue;
        $context_blocks[$r] = _block_render_blocks($blocks);

        // Make blocks editable if allowed.
        if ($this->is_editable_region($r)) {
          foreach ($context_blocks[$r] as $key => $block) {
            $context_blocks[$r][$key] = $this->editable_block($block);
          }
        }

        // Sort blocks.
        uasort($context_blocks[$r], array('context_reaction_block', 'block_sort'));
      }
    }
    return isset($context_blocks[$region]) ? $context_blocks[$region] : array();
  }
}
