jQuery(document).ready(function() {
  var wide = jQuery('.views_slideshow_slide img').width();
    jQuery('.block-boxes-os_boxes_slideshow > div').css('width', wide);
    jQuery('.views-slideshow-controls-text').css('width', wide);
    jQuery('.views_slideshow_cycle_main').css('width', wide);
    jQuery('.views-slideshow-controls-bottom').css('width', wide);
  
//Insert the secondary nav to the content region for smartphone
 jQuery(window).resize(function() {
  if (jQuery(window).width() < 600) {
     jQuery('.region-header-third #block-os-secondary-menu').insertAfter('.region-sidebar-second');
  }

 });

});

