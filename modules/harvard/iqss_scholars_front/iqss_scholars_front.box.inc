<?php
/**
 * @file
 * iqss_scholars_front.box.inc
 */

/**
 * Implements hook_default_box().
 */
function iqss_scholars_front_default_box() {
  $export = array();

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'iqss_scholars_fp_headertext';
  $box->plugin_key = 'simple';
  $box->title = '';
  $box->description = 'Front page header text';
  $box->options = array(
    'body' => array(
      'value' => '<div class="os-topheadings">Personal Websites Exclusively for Harvard Scholars<br> <a href="xx">Not a Harvard Scholar? Find out how to create a similar site at your institution.</a></div>',
      'format' => 'filtered_html',
    ),
    'additional_classes' => '',
  );
  $export['iqss_scholars_fp_headertext'] = $box;

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'iqss_scholars_fp_infoblock';
  $box->plugin_key = 'simple';
  $box->title = '';
  $box->description = 'Scholars Info text with video link';
  $box->options = array(
    'body' => array(
      'value' => '<div class="os-leftcol"><strong>Scholars at Harvard</strong> offers you the next generation scholarly web site. With it, you can easily create and manage a highly dynamic, powerful, and customizable site, with an easy-to-use interface and a look and feel that is uniquely yours.
<ul>
<li><a href="sites/all/themes/custom/scholars_harvard/Info-sheet-Scholars-at-Harvard-H-2011.03.17.pdf">Fact Sheet (PDF)</a></li>
<li><a href="http://player.vimeo.com/video/9887585?title=0&amp;byline=0&amp;portrait=0">Video Intro</a></li>
</ul>
</div>
<div class="os-featuredSites"><a href="/websites"><img src="/profiles/openscholar/themes/custom/scholars_harvard/images/featuredSites.png"></a></div>',
      'format' => 'filtered_html',
    ),
    'additional_classes' => '',
  );
  $export['iqss_scholars_fp_infoblock'] = $box;

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'iqss_scholars_fp_logoblock';
  $box->plugin_key = 'simple';
  $box->title = '';
  $box->description = 'Scholars Logo';
  $box->options = array(
    'body' => array(
      'value' => '<h1>Scholars at Harvard</h1>',
      'format' => 'filtered_html',
    ),
    'additional_classes' => '',
  );
  $export['iqss_scholars_fp_logoblock'] = $box;

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'iqss_scholars_fp_learnmore';
  $box->plugin_key = 'simple';
  $box->title = '';
  $box->description = 'Scholars Learn More Box';
  $box->options = array(
    'body' => array(
      'value' => '<div class="os-rightcol">
<p class="os-leadin">Scholars at Harvard is a free web site building tool available to faculty, graduate students and visiting scholars at Harvard.</p>
<ul>
<li><span>Create a beautiful academic web site in seconds.</span></li>
<li><span>Receive more web visibility.</span></li>
<li><span>Build higher citation counts.</span></li>
<li><span>Customize your site\'s layout and structure, and choose (or design) a unique look and feel</span></li>
<li><span>Watch as your site is automatically updated with the latest web technology.</span></li>
</ul>
<p class="os-learnmore"><a href="#">Learn More</a></p>
</div>',
      'format' => 'filtered_html',
    ),
    'additional_classes' => '',
  );
  $export['iqss_scholars_fp_learnmore'] = $box;

  return $export;
}
