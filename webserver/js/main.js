console.log("Hello World!");

(($) => {
    $("#start-stream").click(() => {
        start_stream();
    });

    $("#stop-stream").click(() => {
        stop_stream();
    });

    $("#reboot").click(() => {
        stop_stream();
        // send a request to reboot the esp32
        $.get("/reboot", () => {});
    });

    $("#get_status").click(() => {
        stop_stream();

        // send a request to get the status of the esp32
        $.get("/status", (data) => {
            console.log(data);
            start_stream();
        });
    });

    function start_stream() {
        $(".stream-overlay").addClass("active");
        $("#stream").attr("src", "/stream");
    }

    function stop_stream() {
        $(".stream-overlay").removeClass("active");
        $("#stream").attr("src", "");
    }
})(jQuery);