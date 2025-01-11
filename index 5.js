let order = new Array(3);
const standardIndex = 8;
const maxIndex = 16;
let index = standardIndex;


document.addEventListener("DOMContentLoaded", () => {
    initWeekBoxes();

    /* panel0 ~ 2까지를 이용하기 위함. 각각 0, 1, 2에 저장 */
    for (let i = 0; i < 3; i++) {
        order[i] = document.getElementById("panel" + (8 + i));
    }
});

// 초기화: 처음 상태의 패널 3 ~ 7번을 초기화 함
function initWeekBoxes() {
    nextDate = new Date(standardDate);
    nextDate.setDate(nextDate.getDate() - 9);

    fillPanels(nextDate, 8, 11, 8);

    setCurrentWeekPanel(document.getElementById('panel9')); // may be problem
}

function toggleDetails() {
    if (!isExpanded) {
        panelDisplayFlex([8, 9, 10]);

        let upDown = document.getElementById('up-down');
        upDown.style.display = 'flex';
        upDown.style.width = '10%';
        document.getElementById('panel-container').style.width = "88%";
        detailsButton = document.getElementById('details-button');
        detailsButton.style.width = "88%";
        detailsButton.textContent = '간단히 보기'

    } else {
        reset();

        panelDisplayFlex([9]);
        let upDown = document.getElementById('up-down');
        upDown.style.display = 'none';
        upDown.style.width = '0%';
        document.getElementById('panel-container').style.width = "100%";
        detailsButton = document.getElementById('details-button');
        detailsButton.style.width = "100%";
        detailsButton.textContent = '자세히 보기'
    }

    isExpanded = !isExpanded;
}

function up() {
    prev();

    if (index == 9) {
        // let infoBoxes = order[2].querySelectorAll('.info-box');
        // infoBoxes[0].classList.add('highlight');
        setCurrentWeekPanel(order[0]);
    }

    if (index == 6) {
        // let infoBoxes = document.getElementById('panel2').querySelectorAll('.info-box');
        // infoBoxes[0].classList.remove('highlight');
        unsetCurrentWeekPanel(order[0]);
    }

    if (index < maxIndex) {
        let downButton = document.getElementById("down");
        downButton.onclick = down;
        downButton.style.backgroundColor = '';
    }

    if (index == 0) {
        let upButton = document.getElementById("up");
        upButton.onclick = null;
        upButton.style.backgroundColor = 'gray';
    }
}

function prev() { /* 위에서 아래순으로 */ /* 이전은 이전 일주일이 중앙으로 이동하는 것을 뜻함 */
    let temp = order[2];

    for (let i = 1; i >= 0; i--) {
        order[i].style.order = i + 1;
        order[i + 1] = order[i];
    }

    temp.style.order = 0;
    order[0] = temp;

    index--;


    temp = index - standardIndex; /* 이전이면 temp = -1, -2, ... */

    let date = new Date(standardDate);
    date.setDate(date.getDate() + 8 * (temp - 1) - 1) /* 이전의 이전 값을 불러와서 채우기 때문에 temp -1 */

    // panel 안의 모든 info-box 요소를 선택
    setPanelContent(date, order[0])
}

function down() {
    next();

    if (index == 7) {
        // let infoBoxes = document.getElementById('panel2').querySelectorAll('.info-box');
        // infoBoxes[0].classList.add('highlight');
        setCurrentWeekPanel(order[2]);
    }

    if (index == 10) {
        // let infoBoxes = document.getElementById('panel2').querySelectorAll('.info-box');
        // infoBoxes[0].classList.remove('highlight');
        // console.log("checked")
        unsetCurrentWeekPanel(order[2]);
    }

    if (index > 0) {
        let upButton = document.getElementById("up");
        upButton.onclick = up;
        upButton.style.backgroundColor = '';
    }

    if (index == maxIndex) {
        let downButton = document.getElementById("down");
        downButton.onclick = null;
        downButton.style.backgroundColor = 'gray';
    }
}

function next() {
    let temp = order[0];

    for (let i = 1; i < 3; i++) {
        order[i].style.order = i - 1;
        order[i - 1] = order[i];
    }

    temp.style.order = 2;
    order[2] = temp;

    index++;


    temp = index - standardIndex;

    let date = new Date(standardDate);
    date.setDate(date.getDate() + 8 * (temp + 1) - 1);

    setPanelContent(date, order[2]);
}

function reset() {
    if (index == standardIndex) return;

    index = standardIndex;

    //// 언젠가는 동기화가 필요하니 reset에서 진행
    for (let i = 0; i < 3; i++) {
        order[i] = document.getElementById("panel" + (8 + i));
        order[i].style.order = i;
    }

    /* standardDate를 이용시 이용중 12시가 지나고 reset을 누를 경우 날짜 반영이 제대로 되지 않음 */
    /* reset, initial의 경우만 앱 시간 초기화 */
    standardDate = new Date();

    setTimeout(() => {
        let nextDate = new Date(standardDate);
        nextDate.setDate(nextDate.getDate() - 25); // 현재 날짜에서 1일 빼기(아래 반복문에서 무조건 1을 더해서)

        setPanels(nextDate, document.getElementsByClassName('for-mobile'))
    }, 0);

    let date = new Date(standardDate);
    date.setDate(date.getDate() - 9);

    // panel 안의 모든 info-box 요소를 선택
    setPanels(date, order);

    setCurrentWeekPanel(order[1]);

    upButton = document.getElementById("up");
    upButton.onclick = up;
    upButton.style.backgroundColor = '';

    upButton = document.getElementById("down");
    upButton.onclick = down;
    upButton.style.backgroundColor = '';
}

// // 페이지 로딩 후 DOMContentLoaded 이벤트에 따라 함수 호출
// document.addEventListener('DOMContentLoaded', initWeekBoxes);