class Slider {
    constructor(startIndex) {
        this.container = document.querySelector("#panel-container");
        this.panels = document.querySelectorAll(".panel");
        this.prevButton = document.querySelector(".btn-prev");
        this.nextButton = document.querySelector(".btn-next");

        this.currentIndex = startIndex;
        this.startX = 0;
        this.currentTranslate = 0;
        this.prevTranslate = 0;
        this.isDragging = false;

        this.order = new Array(5);

        for (let i = 0; i < 5; i++) {
            this.order[i] = document.getElementById("panel" + i);
        }

        this.num = 0;
    }

    updatePosition = () => {
        this.container.style.transform = `translateX(${this.currentTranslate}px)`;
    };

    setPositionByIndex = () => {
        this.currentTranslate = -this.currentIndex * (this.container.offsetWidth + 10); // 10은 gap /* -은 container 내부의 콘텐츠를 왼쪽으로 이동시키는 효과 */
        this.prevTranslate = this.currentTranslate;
        this.updatePosition();
    };

    touchStart = (e) => {
        this.startX = e.type === "touchstart" ? e.touches[0].clientX : e.clientX;
        this.isDragging = true;
        this.container.style.transition = "none";
    };

    touchMove = (e) => {
        if (!this.isDragging) return; /* 마우스를 클릭하지 않고 그냥 움직이는 경우에는 해당 안함을 뜻함*/
        const currentX = e.type === "touchmove" ? e.touches[0].clientX : e.clientX;
        this.currentTranslate = this.prevTranslate + (currentX - this.startX); /* prevTranslate는 setPositionByIndex에서 지정되는데, 드래그 이전 인덱스에 해당하는 이동을 기억하기 위함인듯 */
        this.updatePosition();
    };

    touchEnd = () => {
        this.isDragging = false;
        const movedBy = this.currentTranslate - this.prevTranslate;

        if (movedBy < -50 && this.currentIndex < this.panels.length - 1) this.currentIndex++; /* 오른쪽에서 왼쪽으로 드래그 */
        if (movedBy > 50) this.currentIndex--; /* 왼쪽에서 오른쪽으로 드래그 */

        this.setPositionByIndex();
        this.container.style.transition = "transform 0.5s ease-in-out";

        let temp = this.order[4];
        temp.style.order = 0;
        this.order[3].style.order = 4;
        this.order[4] = this.order[3];
        this.order[2].style.order = 3;
        this.order[3] = this.order[2];
        this.order[1].style.order = 2;
        this.order[2] = this.order[1];
        this.currentIndex = 2;
        this.setPositionByIndex();
        this.container.style.transition = "none";
        this.order[0].style.order = 1;
        this.order[1] = this.order[0];

        this.order[0] = temp;

        this.num--;
        setTimeout(() => {
            const nextDate = new Date();
            nextDate.setDate(nextDate.getDate() + 8 * (this.num - 2) - 1); // 현재 날짜에서 14일 빼기 + 1일 빼기(아래 반복문에서 무조건 1을 더해서)


            let row = temp;
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
        }, 0);
    };

    showPrev = () => {
        if (this.currentIndex > 0) {
            this.currentIndex--;
            this.setPositionByIndex();
        }
    };

    showNext = () => {
        if (this.currentIndex < this.panels.length - 1) {
            this.currentIndex++;
            this.setPositionByIndex();
        }
    };

    activate() {
        this.container.addEventListener("mousedown", this.touchStart); /* container에만 이벤트 리스너 추가, 다른 곳 마우스 클릭해도 작동안함 */
        this.container.addEventListener("mousemove", this.touchMove);
        this.container.addEventListener("mouseup", this.touchEnd);
        this.container.addEventListener("mouseleave", () => this.isDragging && this.touchEnd());

        // Touch events
        this.container.addEventListener("touchstart", this.touchStart);
        this.container.addEventListener("touchmove", this.touchMove);
        this.container.addEventListener("touchend", this.touchEnd);

        // Button events
        this.prevButton.addEventListener("click", this.showPrev);
        this.nextButton.addEventListener("click", this.showNext);

        // Initialize
        this.setPositionByIndex();
    }

    deactivate() {
        this.container.removeEventListener("mousedown", this.touchStart); /* container에만 이벤트 리스너 추가, 다른 곳 마우스 클릭해도 작동안함 */
        this.container.removeEventListener("mousemove", this.touchMove);
        this.container.removeEventListener("mouseup", this.touchEnd);
        this.container.removeEventListener("mouseleave", () => this.isDragging && this.touchEnd());

        // Touch events
        this.container.removeEventListener("touchstart", this.touchStart);
        this.container.removeEventListener("touchmove", this.touchMove);
        this.container.removeEventListener("touchend", this.touchEnd);

        // Button events
        this.prevButton.removeEventListener("click", this.showPrev);
        this.nextButton.removeEventListener("click", this.showNext);

        this.container.style.transform = `translateX(0px)`; /* 자바스크립트로 지정한 놈이 css에서 지정한것보다 우선되 자바스크립트로 설정해야 */
    }
}

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
