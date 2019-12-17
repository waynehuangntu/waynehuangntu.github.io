class Game {
    constructor(main) {
        let g = {
            main: main,
            actions: {},
            keydowns: {},
            state: 1,
            state_START: 1,
            state_RUNNING: 2,
            state_STOP: 3,
            state_GAMEOVER: 4,
            state_UPDATE: 5,
            canvas: document.getElementById("canvas"),
            context: document.getElementById("canvas").getContext("2d"),
            timer: null,
            fps: main.fps,
        }
        Object.assign(this, g)
    }

    draw(paddle, ball, blockList, score) {
        let g = this

        g.context.clearRect(0, 0, g.canvas.width, g.canvas.height)

        g.drawBg()

        g.drawImage(paddle)

        g.drawImage(ball)

        g.drawBlocks(blockList)

        g.drawText(score)
    }

    drawImage(obj) {
        this.context.drawImage(obj.image, obj.x, obj.y)
    }

    drawBg() {
        let bg = imageFromPath(allImg.background)
        this.context.drawImage(bg, 0, 0)
    }

    drawBlocks(list) {
        for (let item of list) {
            this.drawImage(item)
        }
    }

    drawText(obj) {
        this.context.font = '24px Microsoft YaHei'
        this.context.fillStyle = '#fff'

        this.context.fillText(obj.text + obj.allScore, obj.x, obj.y)

        this.context.fillText(obj.textLv + obj.lv, this.canvas.width - 100, obj.y)
    }

    gameOver() {

        clearInterval(this.timer)

        this.context.clearRect(0, 0, this.canvas.width, this.canvas.height)

        this.drawBg()

        this.context.font = '48px Microsoft YaHei'
        this.context.fillStyle = '#fff'
        this.context.fillText('遊戲結束', 404, 226)
    }

    goodGame() {

        clearInterval(this.timer)

        this.context.clearRect(0, 0, this.canvas.width, this.canvas.height)

        this.drawBg()

        this.context.font = '48px Microsoft YaHei'
        this.context.fillStyle = '#fff'
        this.context.fillText('恭喜晉級', 308, 226)
    }

    finalGame() {

        clearInterval(this.timer)

        this.context.clearRect(0, 0, this.canvas.width, this.canvas.height)

        this.drawBg()

        this.context.font = '48px Microsoft YaHei'
        this.context.fillStyle = '#fff'
        this.context.fillText('恭喜通過所有關卡', 308, 226)
    }

    registerAction(key, callback) {
        this.actions[key] = callback
    }

    checkBallBlock(g, paddle, ball, blockList, score) {
        let p = paddle,
            b = ball

        if (p.collide(b)) {

            if (Math.abs(b.y + b.h / 2 - p.y + p.h / 2) > Math.abs(b.y + b.h / 2 + b.speedY - p.y + p.h / 2)) {
                b.speedY *= -1
            } else {
                b.speedY *= 1
            }

            b.speedX = p.collideRange(b)
        }

        blockList.forEach(function(item, i, arr) {
            if (item.collide(b)) {
                if (!item.alive) {
                    arr.splice(i, 1)
                }

                if ((b.y < item.y && b.speedY < 0) || (b.y > item.y && b.speedY > 0)) {
                    if (!item.collideBlockHorn(b)) {
                        b.speedY *= -1
                    } else {
                        b.speedY *= 1
                    }
                } else {
                    b.speedY *= 1
                }

                if (item.collideBlockHorn(b)) {
                    b.speedX *= -1
                }

                score.computeScore()
            }
        })

        if (p.x <= 0) {
            p.isLeftMove = false
        } else {
            p.isLeftMove = true
        }
        if (p.x >= 1000 - p.w) {
            p.isRightMove = false
        } else {
            p.isRightMove = true
        }

        b.move(g)
    }

    setTimer(paddle, ball, blockList, score) {
        let g = this
        let main = g.main
        g.timer = setInterval(function() {

            let actions = Object.keys(g.actions)
            for (let i = 0; i < actions.length; i++) {
                let key = actions[i]
                if (g.keydowns[key]) {

                    g.actions[key]()
                }
            }

            if (blockList.length == 0) {
                if (main.LV === main.MAXLV) {

                    g.state = g.state_UPDATE

                    g.finalGame()
                } else {

                    g.state = g.state_UPDATE

                    g.goodGame()
                }
            }

            if (g.state === g.state_GAMEOVER) {
                g.gameOver()
            }

            if (g.state === g.state_RUNNING) {
                g.checkBallBlock(g, paddle, ball, blockList, score)

                g.draw(paddle, ball, blockList, score)
            } else if (g.state === g.state_START) {

                g.draw(paddle, ball, blockList, score)
            }
        }, 1000 / g.fps)
    }

    init() {
        let g = this,
            paddle = g.main.paddle,
            ball = g.main.ball,
            blockList = g.main.blockList,
            score = g.main.score
            // 设置键盘按下及松开相关注册函数
        window.addEventListener('keydown', function(event) {
            g.keydowns[event.keyCode] = true
        })
        window.addEventListener('keyup', function(event) {
            g.keydowns[event.keyCode] = false
        })
        g.registerAction = function(key, callback) {
                g.actions[key] = callback
            }
            // 注册左方向键移动事件
        g.registerAction('37', function() {
                // 判断游戏是否处于运行阶段
                if (g.state === g.state_RUNNING && paddle.isLeftMove) {
                    paddle.moveLeft()
                }
            })
            // 注册右方向键移动事件
        g.registerAction('39', function() {
            // 判断游戏是否处于运行阶段
            if (g.state === g.state_RUNNING && paddle.isRightMove) {
                paddle.moveRight()
            }
        })
        window.addEventListener('keydown', function(event) {
                switch (event.keyCode) {
                    // 注册空格键发射事件
                    case 32:
                        if (g.state === g.state_GAMEOVER) { // 游戏结束时
                            // 开始游戏
                            g.state = g.state_START
                                // 初始化
                            g.main.start()
                        } else {
                            // 开始游戏
                            ball.fired = true
                            g.state = g.state_RUNNING
                        }
                        break
                        // N 键进入下一关卡
                    case 78:
                        // 游戏状态为通关，且不为最终关卡时
                        if (g.state === g.state_UPDATE && g.main.LV !== g.main.MAXLV) { // 进入下一关
                            // 开始游戏
                            g.state = g.state_START
                                // 初始化下一关卡
                            g.main.start(++g.main.LV)
                        }
                        break
                        // P 键暂停游戏事件
                    case 80:
                        g.state = g.state_STOP
                        break
                }
            })
            // 设置轮询定时器
        g.setTimer(paddle, ball, blockList, score)
    }
}