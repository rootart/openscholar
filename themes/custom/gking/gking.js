jQuery(document).ready(function() {

    jQuery('.front .region-header-second .block-boxes-os_boxes_html .boxes-box div:last').append('<a class="more" href="#">More</a>');

    var container = $("body.front .region-header-second #block-boxes-scholar_biocv_bio");
    container.find('div.node').prepend('<a class="more" href="#">CLOSE X</a>');
    container.find('div.node .content').prepend('<h3 class="cv-direct-download">Full CV: <a href="http://gking.harvard.edu/vitae/vitae.pdf">PDF</a></h3><div class="clear"></div>');


    jQuery(".front .region-header-second a.more").click(function(event){
      container.find('a.more').toggle();
      if (container.is(".bio-open")) {
        container.removeClass("bio-open").animate({height:'0'},"1500");
      } else {
        container.addClass("bio-open").animate({height:'70%'},"1500");
      }
    });
    
    jQuery(".front #pub_type_tabs").tabs({ fx: { opacity: 'toggle' } });
    
    jQuery(".front #header-top .wrap").addClass('clearfix');
    
});
