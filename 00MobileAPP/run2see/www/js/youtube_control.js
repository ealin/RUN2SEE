

        // This code loads the IFrame Player API code asynchronously.
        var tag = document.createElement('script');

        tag.src = "https://www.youtube.com/iframe_api";
        var firstScriptTag = document.getElementsByTagName('script')[0];
        firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);

        // This function creates an <iframe> (and YouTube player)
        //    after the API code downloads.
        var player;
        var playerSatus;

        function onYouTubeIframeAPIReady() {
            player = new YT.Player('player', {
            height: '240',
            width: '320',
            videoId: '2RDkd8nZVhc',
            events: {
                'onReady': onPlayerReady,
                'onStateChange': onPlayerStateChange
            }
            });
        }

        // The API will call this function when the video player is ready.
        function onPlayerReady(event) {
            alert('Ready!!');
            event.target.playVideo();
        }

        // 5. The API calls this function when the player's state changes.
        //    The function indicates that when playing a video (state=1),
        //    the player should play for six seconds and then stop.
        //var done = false;
        function onPlayerStateChange(event) {

            /*
            if (event.data == YT.PlayerState.PLAYING && !done) {
                setTimeout(stopVideo, 6000);
                done = true;
                alert('hello!');
            }*/
            playerSatus = event.data;
        }
        function stopVideo() {
            player.stopVideo();
        }
    


        /* We are doing the event call using jQuery */
        $('#pause').click( function(){
            // Checks for the video is playing or not.
            alert('hello!');
            if (playerSatus == YT.PlayerState.PLAYING) {
                player.pauseVideo(); // 'player' variable we already defined and assigned.
                alert('helloggyy!');
            }
        });
