// 
// OSのバージョンとか本体搭載メモリとか調べる
//
#import <UIKit/UIKit.h>


typedef struct {
	NSUInteger free;								// 未使用領域
	NSUInteger used;								// 使用済み
	NSUInteger unknown;								// 実メモリ - (未使用領域 + 使用済み)
} sOSSysMemory;

extern float osInfoGetVersion(void);

extern NSUInteger osInfoRealMemory(void);
extern void osInfoSysMemory(sOSSysMemory *aOut);
extern NSUInteger osInfoAppMemory(void);

extern BOOL osInfoIsCamera(void);
