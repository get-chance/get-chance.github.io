var standardDate = new Date(); // 앱 기준 시간
var initial = new Map();
var isExpanded = false;

document.addEventListener("DOMContentLoaded", () => {
    if (window.innerWidth < 520) { /* 초기 로드시 모바일일때만, event 등록 등 */
        panelDisplayFlex([0, 1, 2, 3, 4, 5, 6, 7]); // mobile panels displayed
        slider.activate();

    } else {
        panelDisplayFlex([9]);
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

        panelDisplayFlex([0, 1, 2, 3, 4, 5, 6, 7]);

        /* 데스크탑 -> 모바일로 접근시 이벤트 등록 */
        slider.activate();
    } else {
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

function fillPanels(nextDate, start, end, count_in_panel) {
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
}

function setPanelContent(nextDate, panel) {
    const infoBoxes = panel.querySelectorAll('.info-box');

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

function setPanels(nextDate, panels) {
    for (const panel of panels) {
        setPanelContent(nextDate, panel);
    }
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