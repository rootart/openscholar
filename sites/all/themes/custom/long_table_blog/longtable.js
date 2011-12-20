
$(document).ready(function() {
  $('#content-main div.node div.content-more').each(function(){

    var more_region = $(this);
    $(this).siblings('div.links').find('li.node_read_more').children('a').click(function(){
    
      //replace the html
      more_region.siblings('div.content').html( more_region.html() );
    	
      if(more_region.hasClass("complete")){
    	//We are showing the whole body remove the link
    	$(this).remove();
      }else{
    	//Remove the click handler
        $(this).unbind("click");
      }
      
      return false;
    });
	   
  });
  
});