const swiper = new Swiper('.swiper', {
    /*
    // Optional parameters
    direction: 'vertical',
    loop: true,
    */

    // If we need pagination
    pagination: {
      el: '.swiper-pagination',
    },
  
    // Navigation arrows
    navigation: {
      nextEl: '.swiper-button-next',
      prevEl: '.swiper-button-prev',
    },

    on: {
        activeIndexChange: function () {
            const descriptions = [
                { title: '안산시 중앙동:', text: '중앙역 주변 시내입니다.' },
                { title: '안산시 중앙도서관', text: '' },
                { title: '안산시 시화호', text: '' }
            ];
            console.log(this.realIndex+'번째 slide입니다.');
            $('.htit').text(descriptions[this.realIndex].title);
            $('.htxt').text(descriptions[this.realIndex].text);
        }
    }
  
    /*
    // And if we need scrollbar
    scrollbar: {
      el: '.swiper-scrollbar',
    },
    */
  });