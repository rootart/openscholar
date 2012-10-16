<?php

class SpacesOverridesMenuMigration extends SpacesOverridesTableMigration {

  public function __construct() {
    $this->dependencies = array('SpacesOverridesVariable'); //preset needs to be migrated before menus will work
    $this->source_object ='vsite_menu';
    $this->dest_object = 'menus';
    parent::__construct(array('term', 'taxonomy'));
  }

  public function query() {
    $query = parent::query();

    //there's no "Not in a menu" menu anymore.  Drop those items.
    //if there's time later, we could create a new menu for them, but not place the widget.
    $query->condition('object_id', 'none', '!=');
    return $query;
  }



  public function prepareRow(&$row) {
    $value = unserialize($row->value);
    $object_id = $row->object_id;

    $object_id = $this->_update_menu_name($object_id);

    ctools_include('menu', 'os');

    //menu storage has changed a bit.  there's more info now.
    //biggest concern: mlid.  custom menu items forge it.  legit menu items interit it from menu_links.
    //os_menu_link_save <- reference

    //if forging, update vsite_menu_mlid_increment variable (conveniently located in spaces_overrides)

    //lets forge first.  then inherit if there's a good way to do so.
    $new_menu = array();
    foreach ($value as $menu_item) {
      $new_menu_item = $this->_update_menu_item($menu_item, $row->id);
      $id = $new_menu_item['mlid'];
      $new_menu[$id] = $new_menu_item;
    }

    $row->object_id = $object_id;
    //$row->value = serialize($value);
    $row->value = serialize($new_menu);
    return parent::prepareRow($row);
  }

  //clear the menu cache after import.
  public function postImport() {
    parent::postImport();
    drupal_flush_all_caches();
  }

  public function _update_menu_name($name) {
    $menus = array(
      'features' => 'primary-menu',
      'none' => FALSE,
      'scholar_secondary_menu' => 'secondary-menu',
    );

    return (isset($menus[$name])) ? $menus[$name] : FALSE;
  }

  public function _update_menu_item($menu_item, $gid) {
    //update name
    $menu_item['menu_name'] = $this->_update_menu_name($menu_item['menu_name']);
    $menu_item['module'] = 'system';
    $menu_item['router_path'] = _menu_find_router_path($menu_item['link_path']);

    //get or make mlid
//    if (($mlid = os_menu_load_link_path($menu_item['link_path'])) === FALSE) {
      $mlid = $this->_os_migrate_get_mlid($menu_item['link_path'], $gid);
  //  }

    //attach defaults missing from d6
    $menu_item += array(
      'customized' => 0,
      'updated' => 0,
      'hidden' => 0,
      'external' => 0,
      'mlid' => $mlid,
      'plid' => 0,
      'p1' => $mlid,
      'depth' => 1,
      'has_children' => 0,
    );

    foreach (range(2,9) as $n) {
      $menu_item["p$n"] = 0;
    }


    //todo depth, plids

    //plid, p1-9, depth,




    return $menu_item;
  }

  //get a new mlid.  update existing mlid counter
  public function _os_migrate_get_mlid($path, $gid) {
    //lets see if an mlid already exists
    $result = db_select('menu_links', 'ml')
      ->fields('ml', array('mlid'))
      ->condition('link_path', $path, '=')
      ->condition('menu_name', array('primary-menu', 'secondary-menu'), 'IN')
      ->condition('module', 'system', '=')
      //->condition('module', 'menu', '=')
      ->execute();

    if ($result->rowCount()) {
      return $result->fetchField();
    }



//     $mlid = db_query_range("SELECT mlid FROM {menu_links} WHERE link_path = :path AND module = 'menu' AND menu_name IN (:type_menus) ORDER BY mlid ASC", 0, 1, array(
//         ':path' => $path,
//         ':type_menus' => array_keys($os_menus)
//     ))->fetchField();

    $result = db_select('spaces_overrides', 's')
      ->fields('s', array('value'))
      ->condition('id', $gid, '=')
      ->condition('object_type', 'variable', '=')
      ->condition('object_id', 'vsite_menu_mlid_increment', '=')
      ->execute();

    $update = FALSE;
    if ($result->rowCount()) {
      $update = TRUE;
      $mlid = unserialize($result->fetchField());
    } elseif (module_exists('vsite_menu')) {
      $mlid = VSITE_MENU_CUSTOM_MLID;
    } else {
      $mlid = 1000;
    }

    $mlid++;

    //drupal_write_record('spaces_overrides', $record, array('id', 'object_type', 'object_id', 'type'));
    $record = array('value' => serialize($mlid), 'id' => $gid, 'object_type' => 'variable', 'object_id' => 'vsite_menu_mlid_increment', 'type' => 'og');

    if ($update) {
      $updated = db_update('spaces_overrides')
        ->fields($record)
        ->condition('id', $gid, '=')
        ->condition('object_type', 'variable', '=' )
        ->condition('object_id','vsite_menu_mlid_increment', '=')
        ->condition('type', 'og', '=')
        ->execute();
    } else {
      db_insert('spaces_overrides')->fields($record)->execute();
    }

    return $mlid;
  }
}