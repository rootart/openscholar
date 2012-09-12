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

    $vocabularies = $this->get_all_volcabularies();

    $form['vocabulary'] = array(
      '#type' => 'select',
      '#title' => t('Vocabularies'),
      '#options' => $vocabularies,
      '#default_value' => $this->options['settings']['vocabulary'],
      '#required' => TRUE,
      '#description' => t('Which vocabularies would you like to use to filter posts.'),
    );

    $form += parent::options_form($form_state);
    return $form;
  }

  /**
   * Implementation of boxes_content::render().
   */
  public function render() {
    $block = parent::render();
    $block['content'] = "Filter By Category Block [STUB]";
    // For now - display the first vocabulary terms.
    $block['content'] .= theme('volcabulary_term_list', array(
      'vid' => $this->options['settings']['vocabulary'],
      'options' => array(
        'simple_view' => TRUE,
      ),
    ));

    return $block;
  }

  /**
   * Return the cache parameters for this box.
   */
  public function cache_info(){
  }

  /**
   * Get all the volcabularies id using the taxonomy terms.
   */
   private function get_all_volcabularies() {
      $query = db_query('SELECT * FROM {taxonomy_vocabulary}');

      $vocabularies = array();

      foreach ($query->fetchAll() as $result) {
        $vocabularies[$result->vid] = $result->name;
      }

      return $vocabularies;
   }
}