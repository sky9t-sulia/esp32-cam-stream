console.log("Hello World!");

const FRAMESIZE = {
    0: "FRAMESIZE_96X96",
    1: "FRAMESIZE_QQVGA",
    2: "FRAMESIZE_QCIF",
    3: "FRAMESIZE_HQVGA",
    4: "FRAMESIZE_240X240",
    5: "FRAMESIZE_QVGA",
    6: "FRAMESIZE_CIF",
    7: "FRAMESIZE_HVGA",
    8: "FRAMESIZE_VGA",
    9: "FRAMESIZE_SVGA",
    10: "FRAMESIZE_XGA",
    11: "FRAMESIZE_HD",
    12: "FRAMESIZE_SXGA",
    13: "FRAMESIZE_UXGA",
};

const PIXFORMAT = {
    0: "PIXFORMAT_RGB565",    // 2BPP/RGB565
    1: "PIXFORMAT_YUV422",    // 2BPP/YUV422
    2: "PIXFORMAT_YUV420",    // 1.5BPP/YUV420
    3: "PIXFORMAT_GRAYSCALE", // 1BPP/GRAYSCALE
    4: "PIXFORMAT_JPEG",      // JPEG/COMPRESSED
    5: "PIXFORMAT_RGB888",    // 3BPP/RGB888
    6: "PIXFORMAT_RAW",       // RAW
    7: "PIXFORMAT_RGB444",    // 3BP2P/RGB444
    8: "PIXFORMAT_RGB555",    // 3BP2P/RGB555
};

(($) => {
    let stream_playing = false;
    let config = {};

    Object.keys(FRAMESIZE).map((key) => {
        $("#framesize").append(`<option value="${key}">${FRAMESIZE[key]}</option>`);
    });

    Object.keys(PIXFORMAT).map((key) => {
        let disabled = false;
        if (PIXFORMAT[key] !== "PIXFORMAT_JPEG") {
            disabled = true;
        }
        $("#pixformat").append(`<option ${disabled ? "disabled=disabled" : ""} value="${key}">${PIXFORMAT[key]}</option>`);
    });

    $("#quality").change(() => {
        config.quality = parseInt($("#quality").val(), 10);
    });

    $("#framesize").change(() => {
        config.framesize = parseInt($("#framesize").val(), 10);
    });

    $("#pixformat").change(() => {
        config.pixformat = parseInt($("#pixformat").val(), 10);
    });

    $("#set-config").click(() => {
        stop_stream();
        $.post("/config", JSON.stringify(config), (data) => {
            console.log(data);
            start_stream();
        });
    });

    function process_config() {
        $("#quality").val(config.quality);

        $("#framesize").val(config.framesize);
        $("#pixformat").val(config.pixformat);
    }

    $.get("/config", (data) => {
        console.log(data);
        config = data;
        process_config();
    });

    $("#start-stream").click(() => {
        stream_playing = true;
        start_stream();
    });

    $("#stop-stream").click(() => {
        stream_playing = false;
        stop_stream();
    });

    $("#reboot").click(() => {
        stop_stream();
        $.get("/reboot", () => {});
    });

    function start_stream() {
        if (!stream_playing) {
            return;
        }
        $(".stream-overlay").addClass("active");
        $("#stream").attr("src", "/stream");
    }

    function stop_stream() {
        $(".stream-overlay").removeClass("active");
        $("#stream").attr("src", "");
    }
})(jQuery);