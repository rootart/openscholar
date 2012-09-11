<?php

/**
 * @author rbran100
 * @category boxes
 *
 * The class for filtering content by taxonomy.
 */
class os_taxonomy_fbt extends os_boxes_default {

  /**
   * Implementation of boxes_content::options_defaults().
   */
  public function options_defaults(){
    $options = parent::options_defaults();

    return $options;
  }

  /**
   * Implementation of boxes_content::options_form().
   */
  public function options_form(&$form_state){
    $form = array();

    $form += parent::options_form($form_state);
    return $form;
  }

  /**
   * Implementation of boxes_content::render().
   */
  public function render(){
    $block = parent::render();

    $block['content'] = "Filter By Category Block [STUB]";

    return $block;
  }

  /**
   * Return the cache parameters for this box.
   */
  public function cache_info(){
  }
}