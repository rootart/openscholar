Drupal.behaviors.harvard_longtable = function (context){
  
  $('#content-main div.node div.content-more').each(function(){
    $(this).hasClass("complete");
    var more_region = $(this);
    $(this).siblings('div.links').children('li.node_read_more').childeren('a').click(function(){
    
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
  
}