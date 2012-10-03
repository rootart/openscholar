<?php
class SpacesOverridesContextMigration extends SpacesOverridesTableMigration {

  public function __construct(){
    $this->dependencies = array(
      'SpacesOverridesMenu'
    );
    $this->source_object = $this->dest_object = 'context';
    parent::__construct();
  }

  public function query(){
    $query = parent::query();

    $query->join('og', 'og', 'og.nid = s.id');

    /* Only use contexts from the currently selected theme.  In d7 we aren't storing context overrides based on theme anymore, so no use migrating the rest.
     * This is the equivalent of 'WHERE  object_id LIKE CONCAT("%", og_theme) '
     * but dbtng requires the placeholder, even for a string like this. */
    $query->where('object_id LIKE CONCAT(:per, og_theme) ', array(
      ':per' => '%'
    ));

    return $query;
  }

  public function prepareRow(&$row){
    $value = unserialize($row->value);
    $object_id = $row->object_id;

    list($object_id, $value) = $this->_update_context($object_id, $value, $row->id);
    if (! isset($value, $object_id)) {
      return FALSE; //skip this row.
    }

    $row->object_id = $object_id;
    $row->value = serialize($value);
    return parent::prepareRow($row);
  }

  /**
   * @function _update_context
   *
   * @param string $object_id
   *   identifier of context
   * @param array $value
   *   Array of blocks provided by context
   * @param int $nid
   *   Node id of the vsite providing this group
   *
   * @return boolean|array
   *   Returns updated $object_id and $value parameters or FALSE.
   */
  public function _update_context($object_id, array $value, $nid){

    //Update the key to use the new context name.  Or bail out to drop the row.
    if (($object_id = $this->_update_context_key($object_id, $nid)) === FALSE) {
      return FALSE;
    }

    //boxes in values
    foreach ( $value['blocks'] as $id => $block ) {

      list($new_id, $block) = $this->_update_context_block($id, $block);
      unset($value['blocks'][$id]);
      if (isset($new_id)) {
        $value['blocks'][$new_id] = $block;
      }
    }

    //check the three special cases
    if (in_array($nid, array(
      1,
      8,
      10057
    ))) {
      $this->_update_context_edge_cases($object_id, $value, $nid);
    }

    return array(
      $object_id,
      $value
    );
  }

  //updates only the relevant portions of a block for this context
  public function _update_context_block($id, $block){
    $id = $this->_update_context_block_id($id);

    $keep = array(
      'module',
      'delta',
      'region',
      'weight',
      'status',
      'title',
      'plugin',
      'label'
    );
    $block = array_intersect_key($block, array_flip($keep));

    if (in_array($id, array(
      'boxes-scholar_reader-latest_feed_items',
      'scholar_front-get_started',
    ))) {
      //Remove depricated blocks
      return FALSE;
    }

    if ($replacement = $this->_replace_block($id, $block)) {
      return array(
        $id,
        $replacement
      );
    }

    if (isset($block['region'])) {
      $block['region'] = $this->_update_region($block['region']);
    }

    if (isset($block['module'])) {
      $block['module'] = $this->_update_module($block['module']); //always boxes for box plugins
    }

    if (isset($block['delta'])) {
      $block['delta'] = $this->_update_context_delta($block['delta']);
    }

    if (! isset($block['info']) && isset($block['label'])) {
      $block['info'] = $block['label'];
    }

    if (isset($block['plugin'])) {
      require_once (drupal_get_path('module', 'os_migrate') . '/os_migrate_spaces_overrides_boxes.inc');
      $BP = new BoxPorter($this);
      $block['plugin'] = $BP->_update_block_plugin_key($block['plugin']);
    }

    return array(
      $id,
      $block
    );
  }

  function _update_context_block_id($id){
    $id_map = array(
      'vsite_menus-features' => 'os-primary-menu',
      'vsite_menus-scholar_secondary_menu' => 'os-secondary-menu',
      'boxes-scholar_reader-latest_feed_items' => 'views-reader-block',
      'boxes-scholar_biocv_bio' => 'boxes-bio_teaser',
      'boxes-scholar_announcements_latest' => 'boxes-os_announcements_latest',
      'boxes-scholar_blog-blog_rss' => 'boxes-blog_rss_feed',
      'boxes-scholar_presentations_recent' => 'boxes-os_presentations_recent',
      'boxes-scholar_reader-feeds_list' => 'views-reader-block',
      'boxes-vsite_taxonomy_fltr_cat' => 'boxes-os_taxonomy_fltr_cat',
      'boxes-vsite_rss' => 'boxes-os_rss',
      'boxes-scholar_publications_recent' => 'boxes-os_publications_recent',
      'boxes-scholar_book_current_toc' => 'boxes-os_booktoc',
    );

    return (isset($id_map[$id])) ? $id_map[$id] : $id;
  }

  function _update_context_delta($delta){
    $delta_map = array(
      'features' => 'primary-menu',
      'scholar_secondary_menu' => 'secondary-menu',
      'shield_default' => 'shield',
      'scholar_book_recent_docs' => 'os_booklets_recent_docs',
      'scholar_biocv_bio' => 'bio_teaser',
      'scholar_announcements_latest' => 'os_announcements_latest',
      'scholar_blog-blog_rss' => 'blog_rss_feed',
      'scholar_presentations_recent' => 'os_presentations_recent',
      'scholar_reader-feeds_list' => 'reader-block',
      'vsite_taxonomy_fltr_cat' => 'os_taxonomy_fltr_cat',
      'vsite_rss' => 'os_rss',
      'scholar_publications_recent' => 'os_publications_recent',
      'scholar_book_current_toc' => 'os_booktoc',
    );

    return (isset($delta_map[$delta])) ? $delta_map[$delta] : $delta;
  }

  //update a context's key.  return false if we're dropping it.
  public function _update_context_key($object_id, $nid){
    //old to new mapping
    $contexts = array(
      'openscholar-section-sitewide' => FALSE, //no obvious mapping.  nothing in db
      'openscholar-section-sitewide_front' => 'os_sitewide_front',
      'vsite-section-cp' => 'os_cp',
      //'vsite-section-public' => 'os_public', //not used in overrides
      'openscholar-feature-image_gallery' => 'gallery_gallery',
      'openscholar-feature-front' => 'os_front',
      'openscholar-feature-none' => 'os_public' //all sections
    );

    list($context, $action, $type, $theme) = explode(':', $object_id);

    //update the context name
    if (isset($contexts[$context])) {
      $context = $contexts[$context];
    }
    else {
      $context = preg_replace('/openscholar-feature-(\w+)/', '$1_$1', $context); //err?
    }

    //$theme = $this->_update_theme_name($theme, FALSE);
    $object_id = implode(':', array(
      $context,
      $action,
      $type
    ));
    return $object_id;
  }

  /**
   * Perform wholesale replacement of a block
   *
   * @param string $id
   * @return array
   */
  function _replace_block($id, $block = array()){
    $replacements = array(
      'views-reader-block' => array(
        'module' => 'views',
        'delta' => 'reader-block',
        'region' => 'sidebar_second',
        'weight' => '-10',
        'info' => 'Site RSS Feed'
      )
    );

    if (in_array($id, array_keys($replacements))) {
      return $replacements[$id];
    }

    return false;
  }

  /**
   * @function _update_context_edge_cases
   *
   * See https://github.com/openscholar/openscholar/issues/279
   * Three custom themes have been moved down to flavors.  They have
   * special front pages with custom boxes.  Create the context for
   * those here.
   */
  function _update_context_edge_cases($object_id, &$value, $nid){
    if ($object_id != 'os_front:reaction:block') {
      return;
    }

    if (parent::is_domain('http://scholar.harvard.edu')) {

      //only keep these blocks in the context
      $whitelist = array(
        'boxes-site_info',
        'boxes-site_logo',
        'os-primary-menu',
        'boxes-shield',
        'boxes-shield_default',
        'os-secondary-menu'
      );

      //null out the region of these blcoks
      $null_region_list = array(
        //kshepsle
        '1' => array(),
        //rbates
        '8' => array(
          'os-primary-menu'
        ),
        //agordon.net
        '10057' => array(
          'os-primary-menu',
          'boxes-site_info',
          'boxes-site_logo',
          'os-secondary-menu',
          'boxes-shield_default'
        )
      );

      //if blocks above aren't overridden, here they are to include.
      $preset_boxes = array(
        'boxes-site_logo' => array(
          'module' => 'boxes',
          'delta' => 'site_logo',
          'region' => FALSE,
          'weight' => 0,
          'status' => 0,
          'title' => ''
        ),
        'boxes-shield' => array(
          'module' => 'boxes',
          'delta' => 'shield',
          'region' => FALSE,
          'weight' => 0,
          'status' => 0,
          'title' => ''
        ),
        'boxes-site_info' => array(
          'module' => 'boxes',
          'delta' => 'site_info',
          'region' => FALSE,
          'weight' => 0,
          'status' => 0,
          'title' => ''
        ),
        'os-primary-menu' => array(
          'module' => 'os',
          'delta' => 'primary-menu',
          'region' => FALSE,
          'weight' => 0,
          'status' => 0,
          'title' => ''
        ),
        'os-secondary-menu' => array(
          'module' => 'os',
          'delta' => 'secondary-menu',
          'region' => FALSE,
          'weight' => 0,
          'status' => 0,
          'title' => ''
        )
      );

      //add these blocks to context
      $add_blocks = array(
        '1' => array(
          'kshepsle_custom_box' => array(
            'module' => 'boxes',
            'delta' => 'kshepsle_custom_box',
            'weight' => 0,
            'region' => 'content_top',
            'label' => '',
            'info' => 'kshepsle_custom_box'
          )
        ),

        '8' => array(
          'rbates_custom_box' => array(
            'module' => 'boxes',
            'delta' => 'rbates_custom_box',
            'weight' => 0,
            'region' => 'content_top',
            'label' => '',
            'info' => 'rbates_custom_box'
          )
        ),

        '10057' => array(
          'agordon_custom_header_box' => array(
            'module' => 'boxes',
            'delta' => 'agordon_custom_header_box',
            'weight' => 0,
            'region' => 'header_second',
            'label' => '',
            'info' => 'agordon_custom_header_box'
          ),
          'agordon_custom_menu_box' => array(
            'module' => 'boxes',
            'delta' => 'agordon_custom_menu_box',
            'weight' => 0,
            'region' => 'menu_bar',
            'label' => '',
            'info' => 'agordon_custom_menu_box'
          )
        )
      )
      ;

      //gordon, bates need menus explicitly removed from the context.  they're appearing by default.


      //filter out unwanted blocks
      $value['blocks'] = array_intersect_key($value['blocks'], array_flip($whitelist));

      //hide regions by annulling their regions.
      foreach ( $null_region_list[$nid] as $block ) {
        if (isset($value['blocks'][$block])) {
          $value['blocks'][$block]['region'] = FALSE;
        }
        else {
          $values['blocks'][$block] = $preset_boxes[$block];
        }
      }

      //add the custom ones.
      $value['blocks'] += $add_blocks[$nid];
    }
  }
}

