<?php
/**
 * @file
 * iqss_projects_front.box.inc
 */

/**
 * Implements hook_default_box().
 */
function iqss_projects_front_default_box() {
  $export = array();

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'iqss_projects_fp_features';
  $box->plugin_key = 'simple';
  $box->title = '';
  $box->description = 'Features Carousel';
  $box->options = array(
    'body' => array(
      'value' => '<div class="os-row-features">
<h2>Features</h2>
<div class="os-slider">
<div class=" jcarousel-skin-os_front"><div class="jcarousel-container jcarousel-container-horizontal" style="display: block;"><div class="jcarousel-prev jcarousel-prev-horizontal jcarousel-prev-disabled jcarousel-prev-disabled-horizontal" style="display: block;" disabled="true"></div><div class="jcarousel-next jcarousel-next-horizontal" style="display: block;" disabled="false"></div><div class="jcarousel-clip jcarousel-clip-horizontal"><ul id="features-carousel" class="jcarousel-processed jcarousel-list jcarousel-list-horizontal" style="width: 2940px; left: 0px;">
      <li id="feature-academic-content" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-1 jcarousel-item-1-horizontal" jcarouselindex="1">
        <h3>Create anything related to an academic site.</h3>
        <p>Your mission statement, bios, publications, a blog, image galleries, class materials and more.</p>
      </li>
      <li id="word-out" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-2 jcarousel-item-2-horizontal" jcarouselindex="2">
        <h3>Get the word out.</h3>
        <p>Create project announcements, resources, links, RSS feeds.</p>
      </li>
      <li id="feature-look-feel" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-3 jcarousel-item-3-horizontal" jcarouselindex="3">
        <h3>Control the look and feel of your site.</h3>
        <p>Move useful widgets in and out of your site; change menus, and the overall look of your site in seconds.</p>
      </li>
      <li id="feature-events" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-4 jcarousel-item-4-horizontal" jcarouselindex="4">
      <h3>Manage your events</h3>
      <p>Easily build calendars; highlight upcoming events.</p>
    </li>
      <li id="smart-publishing" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-5 jcarousel-item-5-horizontal" jcarouselindex="5">
        <h3>Smart Publishing</h3>
        <p>The built-in content forms and WYSIWYG editor makes it simple to publish. You can begin writing content from almost anywhere on your the site.</p>
      </li>

      <li id="semantic-urls" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-6 jcarousel-item-6-horizontal" jcarouselindex="6">
        <h3>Semantic URLs</h3>
        <p>All pages are given URLs which correspond to the page\'s <strong>title</strong>. No need to suffer with nonsensical URLs such as: "hdc.do?keyword=icomon&amp;tabgroupid=icb.tabgroup50611". Increase visibility and optimize the availability of your sites\' pages in search engines, such as <em>Google</em> and <em>Yahoo!</em> with semantic URLs.</p>
      </li>

      <li id="categorize" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-7 jcarousel-item-7-horizontal" jcarouselindex="7">
        <h3>Category Tagging</h3>
        <p>Organize content and enable visitors to find pages quickly with category tagging. The tools for multi-level category tagging come standard.</p>
      </li>

      <li id="pluggable-features" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-8 jcarousel-item-8-horizontal" jcarouselindex="8">
        <h3>Pluggable Features</h3>
        <p>"Out of the box ready" content features, which provide sections of your site. <strong>Features</strong> can also be set to "private", thus viewable to only members of the project.</p>
      </li>

      <li id="content-aggregation" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-9 jcarousel-item-9-horizontal" jcarouselindex="9">
        <h3>Content Aggregation</h3>
        <p>Keep the web sites fresh by pulling in content from relevant sources using RSS. <strong>Projects at Harvard</strong> provides built-in tools for RSS feeds making feeds easy to set up.</p>
      </li>

      <li id="layout-tool" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-10 jcarousel-item-10-horizontal" jcarouselindex="10">
        <h3>Interactive Layout Tool</h3>
        <p>Design page layouts using a visual "drop-and-drop" blueprint of the site. The Layout Tool provides an easy (and fun!) way to arrange the content presentation on any site. Each site owner can design site-wide default layouts or create unique layouts for different sections of their site.</p>
      </li>

      <li id="sharing" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-11 jcarousel-item-11-horizontal" jcarouselindex="11">
        <h3>Share With Others</h3>
        <p>Easily share web pages on other social networks such as <strong>Facebook</strong> and <strong>Twitter</strong>. Project Managers have the ability to distribute bookmark links to a variety of social network sites quickly and easily.</p>
      </li>

      <li id="analytics" class="jcarousel-item jcarousel-item-horizontal jcarousel-item-12 jcarousel-item-12-horizontal" jcarouselindex="12">
        <h3>Analytics</h3>
        <p>Just provide your Google Analytics ID and data from your <strong>Projects at Harvard</strong> site will be sent to your Goggle analytics account automatically.</p>
      </li>
    </ul></div></div></div><!--CAROUSEL ENDS-->
</div>
</div>',
      'format' => 'filtered_html',
    ),
    'additional_classes' => '',
  );
  $export['iqss_projects_fp_features'] = $box;

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'iqss_projects_fp_infoblock';
  $box->plugin_key = 'simple';
  $box->title = '';
  $box->description = 'Projects Info text';
  $box->options = array(
    'body' => array(
      'value' => '<div class="os-fp-imageholder"><img src="/profiles/openscholar/themes/custom/projects_harvard/images/top_image.png"></div>
<div class="os-more-info"><strong>Projects at Harvard</strong> is a free web site building tool available to groups at Harvard wishing to create a site for their project. It is one of two main Harvard installations of <strong>OpenScholar</strong> and a creation of Harvard’s <em>Institute for Quantitative Social Science.</em><br><br><strong>Projects at Harvard</strong> (and the personal site-oriented <strong>Scholars at Harvard</strong>) provide simple web site creation and management with a suite of features tailored to the particular needs of academics.
<div class="os-big-button"><a href="/site/register">Get your web site!</a></div></div>',
      'format' => 'filtered_html',
    ),
    'additional_classes' => '',
  );
  $export['iqss_projects_fp_infoblock'] = $box;

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'iqss_projects_fp_logoblock';
  $box->plugin_key = 'simple';
  $box->title = '';
  $box->description = 'Projects Logo';
  $box->options = array(
    'body' => array(
      'value' => '<a href="http://projects.iq.harvard.edu/"><img src="/profiles/openscholar/themes/custom/projects_harvard/images/projects-at-harvard-logo.png"></a>',
      'format' => 'filtered_html',
    ),
    'additional_classes' => '',
  );
  $export['iqss_projects_fp_logoblock'] = $box;

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'iqss_projects_fp_navblock';
  $box->plugin_key = 'simple';
  $box->title = '';
  $box->description = 'OpenScholar: Top Links';
  $box->options = array(
    'body' => array(
      'value' => iqss_projects_front_navblock_content(),
      'format' => 'filtered_html',
    ),
    'additional_classes' => '',
  );
  $export['iqss_projects_fp_navblock'] = $box;

  return $export;
}

/**
 * Function to return the contents for iqss_pojects_fp_navblock_view
 */
function iqss_projects_front_navblock_content() {
  $items = array();

  global $user;
  $items[] = array(
    '#title' => $user -> uid ? 'Log out' : 'Log in',
    '#href' =>  $user -> uid ? 'logout' : 'user',
    '#type' => 'link',
  );

  $vsites = vsite_get_vsite_by_owner($user->uid);
  if (!vsite_vsite_exists_access($vsites)) {
    if($vsites){
    	//print_r($vsites);
	    $items[] = array(
		    '#title' => ((count($vsites) > 1) ? 'Go to your sites' : "Go to your site"),
		    '#href' => ((count($vsites) > 1) ? 'user' : $vsites[0]->get_absolute_url()),
	        '#type' => 'link',
		  );
    } //No sites or ablility to create one
  }else{
  	//Create a site
  	$items[] = array(
	    '#title' => ((count($vsites) > 1)?"Create a web site.":'Get your web site!'),
	    '#href' => 'site/register',
  	    '#type' => 'link',
	  );
  }

  $txt = 'Need a Site for a Project?';
  $url = 'http://projects.iq.harvard.edu';
  $mode = variable_get('openscholar_installation_mode', 'scholar');

  if ($mode == 'project'){
    $txt = 'Need a Personal Site?';
    $url = 'http://scholar.harvard.edu';
  }

  $items[] = array(
    '#title' => $txt,
    '#href' =>  $url,
    '#type' => 'link',
  );
  $link_array = array();
  // Loop each item in the array to pass it a new array rendered
  foreach($items as $item) {
  	$link_array[] = array('data'=>drupal_render($item));
  }

  return $vsitesList = theme_item_list(array('items'=>$link_array, 'title'=>'','type'=>'ul','attributes'=>array('id'=>'top-front')));
}
