// 팝업 열기 함수
function openPopupForAddnDelete() {
    document.getElementById('popup-AddnDeleteStock').style.display = 'block';
}

// 팝업 닫기 함수
function closePopupForAddnDelete() {
    document.getElementById('popup-AddnDeleteStock').style.display = 'none';
}

let isExpanded = false;
const initial = new Map();
let order = new Array(5);
let standardDate = new Date(); // 현재 날짜 가져오기
const standardIndex = 8;
const maxIndex = 16;
let index = standardIndex;

for (let i = 0; i < 7; i++) {
    order[i] = document.getElementById("panel" + i);
}


// 초기화: 처음 상태의 5개 패널을 초기화 함
function initWeekBoxes() {
    const nextDate = new Date(standardDate);
    nextDate.setDate(nextDate.getDate() - 17); // 현재 날짜에서 14일 빼기 + 1일 빼기(아래 반복문에서 무조건 1을 더해서)

    for (let i = 0; i < 2; i++) {
        let row = document.getElementById('panel' + i);
        row.innerHTML = '';

        for (let j = 0; j < 8; j++) {
            nextDate.setDate(nextDate.getDate() + 1); // 1일 후의 날짜 계산

            let year = nextDate.getFullYear();
            let month = nextDate.getMonth() + 1;
            let day = nextDate.getDate();

            let text = (day > 20) ? `99+건` : `${day}건`;
            initial.set(`${year}/${month}/${day}`, text); // 초기 상태를 기억하기 위해 버퍼에 저장

            const box = createBox(year, month, day, text);
            row.appendChild(box);
        }
    }

    let row = document.getElementById('panel' + 2);
    row.innerHTML = '';

    for (let j = 0; j < 7; j++) {
        nextDate.setDate(nextDate.getDate() + 1); // 1일 후의 날짜 계산

        let year = nextDate.getFullYear();
        let month = nextDate.getMonth() + 1;
        let day = nextDate.getDate();

        let text = (day > 20) ? `99+건` : `${day}건`;
        initial.set(`${year}/${month}/${day}`, text); // 초기 상태를 기억하기 위해 버퍼에 저장

        const box = createBox(year, month, day, text);
        row.appendChild(box);
    }

    for (let i = 3; i < 7; i++) {
        let row = document.getElementById('panel' + i);
        row.innerHTML = '';

        for (let j = 0; j < 8; j++) {
            nextDate.setDate(nextDate.getDate() + 1); // 1일 후의 날짜 계산

            let year = nextDate.getFullYear();
            let month = nextDate.getMonth() + 1;
            let day = nextDate.getDate();

            let text = (day > 20) ? `99+건` : `${day}건`;
            initial.set(`${year}/${month}/${day}`, text); // 초기 상태를 기억하기 위해 버퍼에 저장

            const box = createBox(year, month, day, text);
            row.appendChild(box);
        }
    }

    setCurrentWeekPanel(document.getElementById('panel2'));
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
    reset();

    if (!isExpanded) {
        order[1].style.display = 'flex';
        order[3].style.display = 'flex';
        let upDown = document.getElementById('up-down');
        upDown.style.display = 'flex';
        upDown.style.width = '10%';
        document.getElementById('panel-container').style.width = "88%";
        detailsButton = document.getElementById('details-button');
        detailsButton.style.width = "88%";
        detailsButton.textContent = '간단히 보기'

    } else {
        order[1].style.display = 'none';
        order[3].style.display = 'none';
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
    let index = prev();

    if (index == 9) {
        let infoBoxes = document.getElementById('panel2').querySelectorAll('.info-box');
        infoBoxes[0].classList.add('highlight');
    }

    if (index == 6) {
        let infoBoxes = document.getElementById('panel2').querySelectorAll('.info-box');
        infoBoxes[0].classList.remove('highlight');
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
    // let temp = order[2];
    // order[1].style.order = 2;
    // order[2] = order[1];
    // order[0].style.order = 1;
    // order[1] = order[0];
    // temp.style.order = 0;
    // order[0] = temp;

    let temp;
    temp = order[4];

    for (let i = 3; i >= 0; i--) {
        order[i].style.order = i + 1;
        order[i + 1] = order[i];
    }

    temp.style.order = 0;
    order[0] = temp;

    index--;


    temp = index - standardIndex; /* 이전이면 temp = -1, -2, ... */
    console.log(temp);

    let date = new Date(standardDate);
    date.setDate(date.getDate() + 8 * (temp - 2) - 1) /* 이전의 이전 값을 불러와서 채우기 때문에 temp -1 */
    console.log(date.getDate());

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
            numberSpan.textContent = day * 1000;

            infoBox.onclick = () => handleBoxClick(year, month, day);
        }
    });

    return index;
}

function down() {
    index = next();

    if (index == 7) {
        let infoBoxes = document.getElementById('panel2').querySelectorAll('.info-box');
        infoBoxes[0].classList.add('highlight');
    }

    if (index == 10) {
        let infoBoxes = document.getElementById('panel2').querySelectorAll('.info-box');
        infoBoxes[0].classList.remove('highlight');
        console.log("checked")
    }

    if (index == maxIndex) {
        let downButton = document.getElementById("down");
        downButton.onclick = null;
        downButton.style.backgroundColor = 'gray';
    }
}

function next() {
    index++;

    // let temp = order[0];
    // order[1].style.order = 0;
    // order[0] = order[1];
    // order[2].style.order = 1;
    // order[1] = order[2];
    // temp.style.order = 2;
    // order[2] = temp;

    let temp = order[0];

    for (let i = 1; i < 5; i++) {
        order[i].style.order = i - 1;
        order[i - 1] = order[i];
    }

    temp.style.order = 4;
    order[4] = temp;


    if (index > 0) {
        let upButton = document.getElementById("up");
        upButton.onclick = up;
        upButton.style.backgroundColor = '';
    }

    temp = index - standardIndex;
    console.log(temp);

    let date = new Date(standardDate);
    date.setDate(date.getDate() + 7 * (temp + 2) - 1);

    // panel 안의 모든 info-box 요소를 선택
    const infoBoxes = order[4].querySelectorAll('.info-box');

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
            numberSpan.textContent = day * 1000;

            infoBox.onclick = () => handleBoxClick(year, month, day);
        }
    });
}

function reset() {
    if (index == standardIndex) return;

    index = standardIndex;

    ////
    for (let i = 0; i < 3; i++) {
        order[i] = document.getElementById("panel" + i);
        order[i].style.order = i;
    }

    const iterator = initial.entries(); // Map의 이터레이터 생성

    for (let i = 0; i < 3; i++) {
        const infoBoxes = order[i].querySelectorAll('.info-box');

        // info-box를 순회하며 작업 수행
        infoBoxes.forEach((infoBox, index) => {
            // date와 number 값 가져오기
            const dateSpan = infoBox.querySelector('.date');
            const numberSpan = infoBox.querySelector('.number');

            if (dateSpan && numberSpan) {
                // 값 변경 예시
                let result = iterator.next().value;
                console.log(result);

                let tokens = result[0].split('/');
                dateSpan.textContent = `${tokens[1]}/${tokens[2]}`;
                numberSpan.textContent = result[1];
                console.log(tokens[0], tokens[1]);

                infoBox.onclick = () => handleBoxClick(tokens[0], tokens[1], tokens[2]);//(standardDate.getFullYear(), tokens[0], tokens[1]);
            }
        });
    }

    let infoBoxes = order[1].querySelectorAll('.info-box');
    infoBoxes[0].classList.add('highlight');

    upButton = document.getElementById("up");
    upButton.onclick = up;
    upButton.style.backgroundColor = '';

    upButton = document.getElementById("down");
    upButton.onclick = down;
    upButton.style.backgroundColor = '';
}

// 페이지 로딩 후 DOMContentLoaded 이벤트에 따라 함수 호출
document.addEventListener('DOMContentLoaded', initWeekBoxes);