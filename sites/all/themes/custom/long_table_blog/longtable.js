
$(document).ready(function() {
  $('#content-main div.node div.content-more').each(function(){

    var more_region = $(this);
    $(this).siblings('div.links').find('li.node_read_more').children('a').click(function(){
    
      //swap the html
      var more_html = more_region.html();      
      more_region.html( more_region.siblings('div.content').html() );
      more_region.siblings('div.content').fadeTo('fast','.33',function () {
        more_region.siblings('div.content').html( more_html );
        more_region.siblings('div.content').fadeTo('slow','1');
      });
      
    	
      if(more_region.hasClass("complete")){
    	//We are showing the whole body remove the link
    	if($(this).hasClass('less')){
    	  $(this).removeClass('less').html("Read more");
    	}else{
    	  $(this).addClass('less').html("Read less");
    	}
      }else{
    	//Remove the click handler
        $(this).unbind("click");
      }
      
      return false;
    });
	   
  });
  
});