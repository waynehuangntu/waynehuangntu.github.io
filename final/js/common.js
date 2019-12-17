const log = console.log.bind(console)

const imageFromPath = function(src) {
    let img = new Image()
    img.src = './images/' + src
    return img
}

const isPageHidden = function(game) {
        let hiddenProperty = 'hidden' in document ? 'hidden' :
            'webkitHidden' in document ? 'webkitHidden' :
            'mozHidden' in document ? 'mozHidden' :
            null
        let visibilityChangeEvent = hiddenProperty.replace(/hidden/i, 'visibilitychange')

        document.addEventListener(visibilityChangeEvent, function() {
            if (!document[hiddenProperty]) {
                setTimeout(function() {
                    game.state = game.state_RUNNING
                }, 100)
            } else {
                game.state = game.state_STOP
            }
        })
    }
    // 圖片素材
const allImg = {
    background: 'background.jpg',
    paddle: 'paddle.png',
    ball: 'ball.png',
    block1: 'block001.png',
    block2: 'block002.png',
}