import Slider from './slider'

document.addEventListener("DOMContentLoaded", () => {
    initWeekBoxes();

    slider = new Slider(2);

    // // Mouse events
    if (window.innerWidth < 520) { /* 초기 로드시 모바일일때만, event 등록 등 */
        slider.activate();
    }
});

/* 화면이 컸을때, details-button을 누르고, 확장된 상태에서 화면을 줄이면, 여전히 확장된 상태로 남아있게 됨.
이를 해결하기 위해, 화면이 작아짐을 감지하고, 확장된 패널을 초기화 후, panel2만 띄우는 것으로 처리를 위함 */
window.addEventListener('resize', () => {
    if (window.innerWidth < 520) {
        console.log('화면 너비가 768px보다 작습니다.');

        /* 데스크탑 -> 모바일로 접근시 이벤트 등록 */
        slider.activate();

        if (isExpanded) {
            reset();

            order[0].style.display = 'none';
            order[2].style.display = 'none';
            let upDown = document.getElementById('up-down');
            upDown.style.display = 'none';
            upDown.style.width = '0%';
            document.getElementById('panel-container').style.width = "100%";
            detailsButton = document.getElementById('details-button');
            detailsButton.style.width = "100%";
            detailsButton.textContent = '자세히 보기'

            isExpanded = !isExpanded;
        }
    } else {
        slider.deactivate();
    }
});
