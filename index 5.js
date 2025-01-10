// import Slider from "slider.js"

const initial = new Map();
let order = new Array(3);
let standardDate = new Date(); // 앱 기준 시간
const standardIndex = 8;
const maxIndex = 16;
let index = standardIndex;
let isExpanded = false;

document.addEventListener("DOMContentLoaded", () => {
    initWeekBoxes();

    slider = new Slider('#panel-container', {
        initialIndex: 3,
        navigation: {
            prevEl: '.btn-prev',
            nextEl: '.btn-next'
        }
    });

    /* panel0 ~ 2까지를 이용하기 위함. 각각 0, 1, 2에 저장 */
    for (let i = 0; i < 3; i++) {
        order[i] = document.getElementById("panel" + (8 + i));
    }

    //
    if (window.innerWidth < 520) { /* 초기 로드시 모바일일때만, event 등록 등 */
        panelDisplayFlex([0, 1, 2, 3, 4, 5, 6, 7]); // mobile panels displayed
        slider.activate();

    } else {
        panelDisplayFlex([9]); // only panel1 displayed

    }
});

/* 화면이 컸을때, details-button을 누르고, 확장된 상태에서 화면을 줄이면, 여전히 확장된 상태로 남아있게 됨.
이를 해결하기 위해, 화면이 작아짐을 감지하고, 확장된 패널을 초기화 후, panel2만 띄우는 것으로 처리를 위함 */
window.addEventListener('resize', () => {
    if (window.innerWidth < 520) {
        console.log('화면 너비가 768px보다 작습니다.');

        if (isExpanded) {
            let upDown = document.getElementById('up-down');
            upDown.style.display = 'none';
            upDown.style.width = '0%';
            document.getElementById('panel-container').style.width = "100%";
            detailsButton = document.getElementById('details-button');
            detailsButton.style.width = "100%";
            detailsButton.textContent = '자세히 보기'

            // isExpanded = !isExpanded;
        }

        // setCurrentWeekPanel(document.getElementById('panel3'));

        panelDisplayFlex([0, 1, 2, 3, 4, 5, 6, 7]);

        /* 데스크탑 -> 모바일로 접근시 이벤트 등록 */
        slider.activate();
    } else {
        // setCurrentWeekPanel(document.getElementById('panel9')); // may be problem

        if (isExpanded) {
            panelDisplayFlex([8, 9, 10]); // only panel1 displayed

            let upDown = document.getElementById('up-down');
            upDown.style.display = 'flex';
            upDown.style.width = '10%';
            document.getElementById('panel-container').style.width = "88%";
            detailsButton = document.getElementById('details-button');
            detailsButton.style.width = "88%";
            detailsButton.textContent = '간단히 보기'
        } else {
            panelDisplayFlex([9]);
        }

        slider.deactivate();
    }
});

// 초기화: 처음 상태의 패널 3 ~ 7번을 초기화 함
function initWeekBoxes() {
    let nextDate = new Date(standardDate);
    nextDate.setDate(nextDate.getDate() - 25); // 현재 날짜에서 1일 빼기(아래 반복문에서 무조건 1을 더해서)

    fillPanels(true, nextDate, 0, 3, 8);
    fillPanels(true, nextDate, 3, 4, 7);
    fillPanels(true, nextDate, 4, 8, 8);

    setCurrentWeekPanel(document.getElementById('panel3'));

    nextDate = new Date();
    nextDate.setDate(nextDate.getDate() - 9);

    fillPanels(true, nextDate, 8, 11, 8);

    setCurrentWeekPanel(document.getElementById('panel9')); // may be problem
}

function fillPanels(create, nextDate, start, end, count_in_panel) {
    if (create == true) {
        for (let i = start; i < end; i++) {
            let row = document.getElementById('panel' + i);
            row.innerHTML = '';

            for (let j = 0; j < count_in_panel; j++) {
                nextDate.setDate(nextDate.getDate() + 1); // 1일 후의 날짜 계산

                let year = nextDate.getFullYear();
                let month = nextDate.getMonth() + 1;
                let day = nextDate.getDate();
                let text = (day > 20) ? `99+건` : `${day}건`;

                const box = createBox(year, month, day, text);
                row.appendChild(box);
            }
        }
    } else {
        for (let i = start; i < end; i++) {
            let row = document.getElementById('panel' + i);
            const infoBoxes = row.querySelectorAll('.info-box');

            // info-box를 순회하며 작업 수행
            infoBoxes.forEach((infoBox, index) => {
                // date와 number 값 가져오기
                const dateSpan = infoBox.querySelector('.date');
                const numberSpan = infoBox.querySelector('.number');

                if (dateSpan && numberSpan) {
                    // 값 변경 예시
                    nextDate.setDate(nextDate.getDate() + 1); // i일 후의 날짜 계산

                    let year = nextDate.getFullYear();
                    let month = nextDate.getMonth() + 1;
                    let day = nextDate.getDate();

                    dateSpan.textContent = `${month}/${day}`;
                    numberSpan.textContent = (day > 20) ? `99+건` : `${day}건`;

                    infoBox.onclick = () => handleBoxClick(year, month, day);
                }
            });
        }

    }
}

function panelDisplayFlex(nums) {
    const panels = document.querySelectorAll(".panel");

    for (let i = 0; i < panels.length; i++) {
        if (nums.includes(i)) {
            console.log(i);
            panels[i].style.display = 'flex';
        } else {
            panels[i].style.display = 'none';
        }
    }
}

function setCurrentWeekPanel(panel) {
    infoBoxes = panel.querySelectorAll('.info-box');
    infoBoxes[0].classList.add('highlight');

    infoBoxes.forEach((box) => {
        box.classList.add('current-week');
    });
}

function unsetCurrentWeekPanel(panel) {
    infoBoxes = panel.querySelectorAll('.info-box');
    infoBoxes[0].classList.remove('highlight');

    infoBoxes.forEach((box) => {
        box.classList.remove('current-week');
    });
}

// 박스 생성 함수
function createBox(year, month, day, value) {
    const box = document.createElement('div');
    box.className = 'info-box';
    box.onclick = () => handleBoxClick(year, month, day);
    box.innerHTML = `<span class="date">${month}/${day}</span> <span class="number">${value}</span>`;
    return box;
}

function handleBoxClick(year, month, day) {
    alert(`${year}년 ${month}월 ${day}일 을 선택하셨습니다`);
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
    const infoBoxes = order[0].querySelectorAll('.info-box');

    // info-box를 순회하며 작업 수행
    infoBoxes.forEach((infoBox, index) => {
        // date와 number 값 가져오기
        const dateSpan = infoBox.querySelector('.date');
        const numberSpan = infoBox.querySelector('.number');

        if (dateSpan && numberSpan) {
            // 값 변경 예시
            date.setDate(date.getDate() + 1); // i일 후의 날짜 계산

            let year = date.getFullYear(); // handleBoxClick에 직접 입력시 12/31이 2025년으로 출력되는 문제
            // 즉, let 변수에 넣고, 이를 함수에 전달해야 제대로 작동(closure 관련 문제)
            let month = date.getMonth() + 1;
            let day = date.getDate();

            dateSpan.textContent = `${month}/${day}`;
            numberSpan.textContent = (day > 20) ? `99+건` : `${day}건`;

            infoBox.onclick = () => handleBoxClick(year, month, day);
        }
    });
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

    // panel 안의 모든 info-box 요소를 선택
    const infoBoxes = order[2].querySelectorAll('.info-box');

    // info-box를 순회하며 작업 수행
    infoBoxes.forEach((infoBox, index) => {
        // date와 number 값 가져오기
        const dateSpan = infoBox.querySelector('.date');
        const numberSpan = infoBox.querySelector('.number');

        if (dateSpan && numberSpan) {
            // 값 변경 예시
            date.setDate(date.getDate() + 1); // i일 후의 날짜 계산

            let year = date.getFullYear();
            let month = date.getMonth() + 1;
            let day = date.getDate();

            dateSpan.textContent = `${month}/${day}`;
            numberSpan.textContent = (day > 20) ? `99+건` : `${day}건`;

            infoBox.onclick = () => handleBoxClick(year, month, day);
        }
    });
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

        fillPanels(false, nextDate, 0, 8);
    }, 0);

    let date = new Date(standardDate);
    date.setDate(date.getDate() - 9);

    // panel 안의 모든 info-box 요소를 선택
    for (let i = 0; i < 3; i++) {
        const infoBoxes = order[i].querySelectorAll('.info-box');

        // info-box를 순회하며 작업 수행
        infoBoxes.forEach((infoBox, index) => {
            // date와 number 값 가져오기
            const dateSpan = infoBox.querySelector('.date');
            const numberSpan = infoBox.querySelector('.number');

            if (dateSpan && numberSpan) {
                // 값 변경 예시
                date.setDate(date.getDate() + 1); // i일 후의 날짜 계산

                let year = date.getFullYear();
                let month = date.getMonth() + 1;
                let day = date.getDate();

                dateSpan.textContent = `${month}/${day}`;
                numberSpan.textContent = (day > 20) ? `99+건` : `${day}건`;

                infoBox.onclick = () => handleBoxClick(year, month, day);
            }
        });
    }

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