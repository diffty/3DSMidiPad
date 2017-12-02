#ifndef THINGS_STORE_HPP
#define THINGS_STORE_HPP

#include "paginated_widget_layout.hpp"
#include "ts_draggable_thing.hpp"
#include "grid_widget_layout.hpp"
#include "image_button_widget.hpp"
#include "ts_things_manager.hpp"
#include "ts_things_store_item.hpp"


class ThingsStorePage;

class ThingsStore : public PaginatedWidgetLayout
{
private:
	uint m_uNbCellX;
	uint m_uNbCellY;

	ImageButtonWidget* m_pPageUpStoreBtn;
	ImageButtonWidget* m_pPageDownStoreBtn;

public:
	ThingsStore(vect2df_t vPos, size2df_t sSize, uint uNbCellX, uint uNbCellY);
	~ThingsStore();
	
	void addPageBtnComponentsToMainScene();
	void addThingToStore(DraggableThing* pThing);
	void createNewPage();
	bool findThingInPages(DraggableThing* pThing, ThingsStorePage** pLayoutWithThing, int* piThingWidgetId);
	DraggableThing* renewThing(DraggableThing* pThingToRenew);
	void replaceThingInStore(DraggableThing* pThingToRenew);

	void receiveTouchInput(vect2d_t touchPt);
	
	static void onPressPageUpBtnCallback(void* pArg);
	static void onPressPageDownBtnCallback(void* pArg);
};


#endif