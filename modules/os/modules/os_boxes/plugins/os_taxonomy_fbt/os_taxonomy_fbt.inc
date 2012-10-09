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
  public function options_defaults() {
    $options = parent::options_defaults();

    $options['vocabulary'] = false;
    $options['bundles'] = array('--all--');
    $options['range'] = 10;
    $options['offset'] = 0;
    $options['show_empty_terms'] = false;
    $options['terms_description'] = false;
    $options['show_childs'] = true;
    $options['add_childs'] = false;
    $options['depth'] = 0;

    return $options;
  }

  /**
   * Implementation of boxes_content::options_form().
   */
  public function options_form(&$form_state) {
    $form = parent::options_form($form_state);

    ctools_include('dependent');
    ctools_add_js('dependent');

    $form = $this->form_elements();

    // Reset the keys.
    foreach (array_keys($form) as $setting) {
      $setting_value = !empty($this->options[$setting]) ? $this->options[$setting] : NULL;
      $form[$setting]['#default_value'] = $setting_value;
    }
    $form += parent::options_form($form_state);
    return $form;
  }

  /**
   * Returning the form for building the form and reset the form values with
   * $form keys.
   */
  public function form_elements() {
    $form = array();
    $bundles = array(
      '--all--' => t('All'),
    ) + self::get_all_bundles();

    $form['vocabulary'] = array(
      '#type' => 'select',
      '#description' => t('Which vocabularies would you like to use to filter posts.'),
      '#title' => t('Vocabularies'),
      '#options' => self::get_all_volcabularies(),
      '#required' => TRUE,
    );

    $form['bundles'] = array(
      '#type' => 'select',
      '#title' => t('Bundle types'),
      '#options' => $bundles,
      '#description' => t('The bundles type of the tagged node that will include in the count'),
      '#multiple' => TRUE,
      '#size' => count(self::get_all_bundles()) / 2,
    );

    $form['range'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of terms to display'),
      '#description' => t('Maximum number of terms to display.'),
    );

    $form['offset'] = array(
      '#type' => 'textfield',
      '#title' => t('Offset'),
      '#description' => t("The term location that the query start from."),
    );

    $form['show_empty_terms'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show empty terms'),
      '#description' => t('Should terms be shown that do not have any posts tagged to them.'),
    );

    $form['terms_description'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show term descriptions'),
      '#description' => t('Show the description of the term if avalible under each term.'),
    );

    $form['show_childs'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show child terms'),
      '#description' => t('Controll if to show the childrens with their father term.'),
    );

    $form['add_childs'] = array(
      '#type' => 'checkbox',
      '#title' => t('Add childs to the count'),
      '#description' => t('Define if we add the childs tagged node number to the count.'),
      '#dependency' => array(
        'edit-show-childs' => array(1),
      ),
    );

    $form['depth'] = array(
      '#type' => 'select',
      '#title' => ('Taxonomy tree depth.'),
      '#description' => t('The maximum term level in the tree to count the tagged nodes.'),
      '#options' => array(
        0 => t("Show all childeren"),
        1 => t("Show only first level"),
        2 => t("2nd Level"),
        3 => t("3rd Level"),
        4 => t("4th Level"),
      ),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array(
        'edit-add-childs' => array(1),
      ),
    );

    return $form;
  }

  /**
   * Implementation of boxes_content::render().
   */
  public function render() {
    // Reset the keys.
    foreach (array_keys($this->form_elements()) as $setting) {
      $this->options['settings'][$setting] = !empty($this->options['settings'][$setting]) ? $this->options['settings'][$setting] : NULL;
    }

    $block = parent::render();
    // For now - display the first vocabulary terms.
    if (isset($this->options['bundles']['--all--'])) {
      $this->options['bundles'] = array();
    }

    $block['content'] = theme('os_taxonomy_vocabulary_item_list', array(
      'vid' => $this->options['vocabulary'],
      'options' => $this->options,
    ));

    return $block;
  }

  /**
   * Return the cache parameters for this box.
   */
  public function cache_info() {
  }

  /**
   * Get all the volcabularies id using the taxonomy terms.
   */
  private function get_all_volcabularies() {
    $taxonomy_vocabularies = taxonomy_get_vocabularies();
    $vocabularies = array();

    foreach ($taxonomy_vocabularies as $taxonomy_vocabulary) {
      $vocabularies[$taxonomy_vocabulary->vid] = $taxonomy_vocabulary->name;
    }

    return $vocabularies;
  }

  /**
   * Return a list of node bundles.
   */
  private function get_all_bundles() {
    $entity = entity_get_info('node');

    $bundles = array();

    foreach ($entity['bundles'] as $machine_name => $bundle) {
      $bundles[$machine_name] = $bundle['label'];
    }

    return $bundles;
  }
}
