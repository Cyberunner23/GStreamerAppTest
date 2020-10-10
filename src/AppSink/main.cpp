#include <iostream>
#include <string>
#include <memory>

#include <gst/gst.h>
#include <string.h>
#include <vector>

#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

class GMainLoopPtr
{
private:
    GMainLoop* _loop;
public:
    GMainLoopPtr() { _loop = g_main_loop_new(nullptr, FALSE); }
    ~GMainLoopPtr() { g_main_loop_unref(_loop); }
    GMainLoop* get() { return _loop; }
};

struct ApplicationCtx
{
    GMainLoopPtr loop;
    GstElement* gstPipeline;

    ~ApplicationCtx()
    {
        if (gstPipeline != nullptr)
        {
            gst_object_unref(gstPipeline);
        }
    }

    gboolean OnBusMessage(GstBus* bus, GstMessage* message)
    {
        GstElement *source;
        switch (GST_MESSAGE_TYPE (message))
        {
            case GST_MESSAGE_EOS:
                std::cout << "The source got dry" << std::endl;
                source = gst_bin_get_by_name (GST_BIN (this->gstPipeline), "testsource");
                gst_app_src_end_of_stream (GST_APP_SRC (source));
                gst_object_unref (source);
                break;
            case GST_MESSAGE_ERROR:
                std::cout << "Received error" << std::endl;
                g_main_loop_quit (this->loop.get());
                break;
            default:
                break;
        }

        return TRUE;
    }

    GstFlowReturn OnNewSampleFromSink(GstElement* elem)
    {
        GstSample *sample;
        GstBuffer *app_buffer, *buffer;
        GstElement *source;
        GstFlowReturn ret;

        /* get the sample from appsink */
        sample = gst_app_sink_pull_sample (GST_APP_SINK (elem));
        buffer = gst_sample_get_buffer (sample);

        GstMapInfo info;
        gst_buffer_map(buffer, &info, GST_MAP_READ);
        std::vector<guint8> buffer_copy(info.data, info.data + info.size);
        gst_buffer_unmap(buffer, &info);
        // AT THIS POINT, WE HAVE THE FRAME IN buffer_copy




        return GST_FLOW_OK;
    }
};

static gboolean on_source_message (GstBus * bus, GstMessage * message, ApplicationCtx* ctx)
{
    return ctx->OnBusMessage(bus, message);
}

static GstFlowReturn on_new_sample_from_sink (GstElement* elem, ApplicationCtx* ctx)
{
    return ctx->OnNewSampleFromSink(elem);
}


int main(int argc, char** argv)
{
    gst_init(&argc, &argv);

    // Create Application Context
    std::unique_ptr<ApplicationCtx> mainCtxPtr = std::make_unique<ApplicationCtx>();

    // Create GST Pipeline
    const std::string pipelineDescription = "videotestsrc ! appsink caps=\"video/x-raw,format=RGB\" name=\"testsink\"";
    mainCtxPtr->gstPipeline = gst_parse_launch(pipelineDescription.c_str(), nullptr);
    if (mainCtxPtr->gstPipeline == nullptr)
    {
        std::cout << "Failed to create GST Pipeline" << std::endl;
        return -1;
    }

    // Register handler on the GST Pipeline message bus
    GstBus* bus = gst_element_get_bus(mainCtxPtr->gstPipeline);
    gst_bus_add_watch(bus, (GstBusFunc)on_source_message, mainCtxPtr.get());
    gst_object_unref(bus);

    GstElement *testsink = gst_bin_get_by_name (GST_BIN (mainCtxPtr->gstPipeline), "testsink");
    g_object_set (G_OBJECT (testsink), "emit-signals", TRUE, "sync", FALSE, NULL);
    g_signal_connect (testsink, "new-sample",
    G_CALLBACK (on_new_sample_from_sink), mainCtxPtr.get());
    gst_object_unref (testsink);

    // Set GST Pipeline to playing
    gst_element_set_state(mainCtxPtr->gstPipeline, GST_STATE_PLAYING);

    std::cout << "Starting main loop" << std::endl;
    g_main_loop_run(mainCtxPtr->loop.get());

    std::cout << "Stopped!" << std::endl;
    return 0;
}