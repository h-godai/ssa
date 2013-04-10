#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <math.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

// デバッグ用メッセージ

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

#define POINT_MAX (5)
#define TRUE (1)
#define FALSE (0)

/**
 * 再開に必要な保存すべきデータ
 */

typedef struct _TouchPoint{
    int32_t using;
    float angle;
    int32_t x;
    int32_t y;
} TouchPoint;

typedef struct _saved_state
{
    TouchPoint point[POINT_MAX];
} saved_state2;

/**
 * アプリケーション内で共通して利用する情報
 */
struct engine
{
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    saved_state2 state;
};

void gluPerspective(double fovy, double aspect, double zNear, double zFar) {
    GLfloat xmin, xmax, ymin, ymax;
    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;
    glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}

#define LENGTH (150)
short triangleBuffer[] = {
/*        X                Y          Z */
-LENGTH / 2, -LENGTH / 2, 0, LENGTH - LENGTH / 2, -LENGTH / 2, 0, -LENGTH
        / 2, LENGTH - LENGTH / 2, 0, LENGTH - LENGTH / 2, LENGTH - LENGTH
        / 2, 0, };

float colorBuffer[] = {
/*   R    G    B    A  */
1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.5, 0.5, 0.0,
        1.0, };

static float angleSpeed = 2;

/**
 * 表示の初期化
 */
void initBox(struct engine* engine) {

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glClearColor(.7f, .7f, .9f, 1.f);
    glShadeModel(GL_SMOOTH);

    float ratio = (float) engine->width / (float) engine->height;
    glViewport(0, 0, (int) engine->width, (int) engine->height);

    // 表示の初期化(毎フレーム)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0, (float)engine->width, 0, (float)engine->height, 0, 100);

}

void drawBox(struct engine* engine) {

    // 表示の初期化(毎フレーム)

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    int i;
    for (i = 0; i < POINT_MAX; i++){

        TouchPoint *tp = &engine->state.point[i];

        if (tp->using == FALSE)
            break;

        if (tp->using == TRUE){
            glPushMatrix();
            glTranslatef((float)tp->x, (float)(engine->height - tp->y), 0);

            glRotatef(tp->angle, 0, 0, 1.0f);

            // 描画する
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            glVertexPointer(3, GL_SHORT, 0, triangleBuffer);
            glColorPointer(4, GL_FLOAT, 0, colorBuffer);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glDisableClientState(GL_VERTEX_ARRAY);
            glPopMatrix();
        }
    }


}

/**
 * デバイスに対してのEGLコンテキストの初期化
 */
static int engine_init_display(struct engine* engine) {
    // OepGL ES と EGLの初期化

    const EGLint attribs[] =
            { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8,
                    EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_NONE };
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window,
            NULL);
    context = eglCreateContext(display, config, NULL, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    int i;
    for (i = 0; i < POINT_MAX; i++){
        engine->state.point[i].angle = 0;
        engine->state.point[i].using = FALSE;
    }

    // ボックス表示の初期化
    initBox(engine);

    return 0;
}

/*
 * 毎フレーム描画
 */
static void engine_draw_frame(struct engine* engine) {

    if (engine->display == NULL) {
        // displayが無い
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawBox(engine);
    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * EGLコンテキストを破棄する
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}


/**
 * 入力イベントを処理する
 */
static int32_t engine_handle_input(struct android_app* app,
        AInputEvent* event) {
    struct engine* engine = (struct engine*) app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        engine->animating = 1;

        int32_t keycode = AKeyEvent_getKeyCode(event);
        if(keycode == AKEYCODE_VOLUME_UP){
            angleSpeed += 0.5f;
        }
        if(keycode == AKEYCODE_VOLUME_DOWN){
            angleSpeed -= 0.5f;
        }
        return 1;
    }

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        engine->animating = 1;

        int i;
        size_t count = AMotionEvent_getPointerCount(event);
        if(count > POINT_MAX)
            count = POINT_MAX;

        for (i = 0; i < POINT_MAX; i++){
            engine->state.point[i].using = FALSE;
        }
        for (i = 0; i < count; i++){
            engine->state.point[i].using = TRUE;

            engine->state.point[i].x = AMotionEvent_getX(event, i);
            engine->state.point[i].y = AMotionEvent_getY(event, i);
            LOGI("touch point(%d): x=%d y=%d", i, engine->state.point[i].x, engine->state.point[i].y);
        }

        return 1;
    }
    return 0;
}

/**
 * メインコマンドの処理
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*) app->userData;
    switch (cmd) {
    case APP_CMD_SAVE_STATE:
        engine->app->savedState = malloc(sizeof(saved_state2));
        *((saved_state2*) engine->app->savedState) = engine->state;
        engine->app->savedStateSize = sizeof(saved_state2);
        break;
    case APP_CMD_INIT_WINDOW:
        if (engine->app->window != NULL) {
            engine_init_display(engine);
            engine_draw_frame(engine);
        }
        break;
    case APP_CMD_TERM_WINDOW:
        engine_term_display(engine);
        break;
    case APP_CMD_GAINED_FOCUS:
        if (engine->accelerometerSensor != NULL) {
            ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                    engine->accelerometerSensor);
            ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                    engine->accelerometerSensor, (1000L / 60) * 1000);
        }
        break;
    case APP_CMD_LOST_FOCUS:
        if (engine->accelerometerSensor != NULL) {
            ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                    engine->accelerometerSensor);
        }
        engine->animating = 0;
        engine_draw_frame(engine);
        break;
    }
}

/**
 * アプリケーション開始
 */
void android_main(struct android_app* state) {
    struct engine engine;

    // glueが削除されないように
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    // センサーからのデータ取得に必要な初期化
    engine.sensorManager = ASensorManager_getInstance();
    engine.accelerometerSensor = ASensorManager_getDefaultSensor(
            engine.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    engine.sensorEventQueue = ASensorManager_createEventQueue(
            engine.sensorManager, state->looper, LOOPER_ID_USER, NULL,
            NULL);



    if (state->savedState != NULL) {
        // 以前の状態に戻す
        engine.state = *(saved_state2*) state->savedState;
    }

    while (1) {

        int ident;
        int events;
        struct android_poll_source* source;

        // アプリケーションが動作することになれば、これらセンサーの制御を行う
        while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL,
                &events, (void**) &source)) >= 0) {

            // イベントを処理する
            if (source != NULL) {
                source->process(state, source);
            }

            // センサーに何かしらのデータが存在したら処理する
            if (ident == LOOPER_ID_USER) {
                if (engine.accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(
                            engine.sensorEventQueue, &event, 1) > 0) {
                        LOGI("accelerometer: x=%f y=%f z=%f",
                                event.acceleration.x, event.acceleration.y,
                                event.acceleration.z);
                    }
                }
            }

            // 破棄要求があったか
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        if (engine.animating) {
            // 次のフレームを描画するのに必要な処理を行う
            int i = 0;
            for (i = 0; i < POINT_MAX; i++){
            engine.state.point[i].angle += angleSpeed;
            if (engine.state.point[i].angle > 360)
                engine.state.point[i].angle = 0;
            }
            // 画面描画
            engine_draw_frame(&engine);
        }
    }
}
