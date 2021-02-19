#ifdef DEBUG
#include "DSManager.h"

int main()
{
	DSManager ds4manager;
	ds4manager.GetDevice();
	int a = 1;
	while ( a)
	{
		ds4manager.InputUpdate();
		Sleep( 10 );
	}
	
	ds4manager.Destroy();

	return 0;
}
#endif