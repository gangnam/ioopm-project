void RemoveFromFreelist(Memory mem, chunk c){
  private_manual *d = (private_manual*) (&mem - sizeof(void*));
  Freelist list = d->flist;
  if (list->current->start == c->start){
    d->flist = list->after;
  }else{
    Freelist prev = list;
    while (list){
      if(list->current->start == c->start){
	prev->after = list->after;
	
      }else{
	prev = list;
	list = list->after;
	
      }
    }
  }
}

void InsertToFreelist(Memory mem, chunk c){
  private_manual *d = (private_manual*) (&mem - sizeof(void*));
    Freelist list = d->flist;
    Freelist prev = list;
    Freelist new;
    int i = 0;
    if (list->INTEGER! == 1){
      while(list) {
        if (list->current->size < c->size) {
	  prev = list;
	  list = list->after;
	}
        else {
	  new->current = c;
	  new->after = list->after;
	  list->after = new;
	  i++;
	  break;
	}
      }
      if (i == 0){
    new->current = c;
    new->after = prev->after;
    prev->after = new;
    
      }
    }else if(list->INTEGER! == 2){
while(list) {
        if (list->current->size < c->size) {
	  prev = list;
	  list = list->after;
	}
        else {
	  new->current = c;
	  new->after = list->after;
	  list->after = new;
	  i++;
	  break;
	}
      }
      if (i == 0){
    new->current = c;
    new->after = prev->after;
    prev->after = new;
    
      }
    }
}
    
    

