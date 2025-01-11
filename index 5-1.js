// import Slider from "slider.js"

var slider;

document.addEventListener("DOMContentLoaded", () => {
    initWeekBoxesForMobile();

    slider = new Slider('#panel-container', {
        initialIndex: 3,
        navigation: {
            prevEl: '.btn-prev',
            nextEl: '.btn-next'
        }
    });
});

// 초기화: 처음 상태의 패널 3 ~ 7번을 초기화 함
function initWeekBoxesForMobile() {
    let nextDate = new Date(standardDate);
    nextDate.setDate(nextDate.getDate() - 25); // 현재 날짜에서 1일 빼기(아래 반복문에서 무조건 1을 더해서)

    fillPanels(nextDate, 0, 3, 8);
    fillPanels(nextDate, 3, 4, 7);
    fillPanels(nextDate, 4, 8, 8);

    setCurrentWeekPanel(document.getElementById('panel3'));
}

