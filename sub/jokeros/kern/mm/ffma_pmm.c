#include <pmm.h>
#include <list.h>
#include <string.h>
#include <ffma_pmm.h>
#include <stdio.h>

free_area_t free_area;

#define free_list (free_area.free_list)
#define nr_free (free_area.nr_free)

static void 
ffma_init(void) {
	list_init(&free_list);
	nr_free = 0;
}

static void 
ffma_init_memmap(struct Page *base, size_t n) {
	cprintf("ffma_init_memmap,base:%d,size:%d\n",base,n);
	struct Page *p=base;
	for(;p!=base+n;p++) {
		p->flags = 0;
		p->property = 0;
		set_page_ref(p,0);
	}
	base->property = n;
	SetPageProperty(base);
	nr_free += n;
	list_add(&free_list, &(base->page_link));
}
	
static struct Page *
ffma_alloc_pages(size_t n) {
	list_entry_t *le = &free_list;
	struct Page *page = NULL;
	while((le=list_next(le)) != &free_list) {
		struct Page *p = le2page(le, page_link);
		if(p->property >= n){
			page = p;
			break;
		}
	}
	if(page->property > n) {
		struct Page *p = page + n;
		p->property = page->property - n;
		list_add(&free_list, &(p->page_link));
	}
	nr_free -= n;
	list_del(&(page->page_link));
	return page;
}

static void 
ffma_free_pages(struct Page *base, size_t n) {
	struct Page *p = base;
	for(;p!=base+n;p++) {
		p->flags = PG_property;
		p->property = 0;
		set_page_ref(p,0);
	}
	base->property = n;
	SetPageProperty(base);
	nr_free += n;
	list_entry_t *le = list_next(&free_list);
	while(le != &free_list){
		p = le2page(le, page_link);
		le = list_next(le);
		if(p == base + base->property) {
			base->property += p->property;
			list_del(&(p->page_link));
		}
		else if(p + p->property == base) {
			p->property += n;
			base = p;
			list_del(&(p->page_link));
		}
	}
	list_add(&free_list, &(base->page_link));
		
}

static size_t
ffma_nr_free_pages(void) {
	cprintf("ffma_nr_free_pages,nr_free:%d\n",nr_free);
	return 0;
}

static void
ffma_check(void) {
	cprintf("ffma_check\n");
}

const struct pmm_manager ffma_pmm_manager = {
	.name = "ffma_pmm_manager",
	.init = ffma_init,
	.init_memmap = ffma_init_memmap,
	.alloc_pages = ffma_alloc_pages,
	.free_pages = ffma_free_pages,
	.nr_free_pages = ffma_nr_free_pages,
	.check = ffma_check,
};

