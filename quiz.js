// 퀴즈 데이터
const quizzes = [
  {
      question: "1번 퀴즈: 안산시와 가장 가까운 도시는?",
      options: ["양평", "구리", "군포"],
      answer: "C"
  },
  {
      question: "2번 퀴즈: 오늘날의 안산시가 된것은 1850년 12월이다.",
      options: ["O", "X"],
      answer: "B"
  },
  {
      question: "3번 퀴즈: 안산시의 맛집으로는 귀큰여우가 있다",
      options: ["O", "X"],
      answer: "A"
  },
];
  
let currentQuizIndex = 0;
var score = 0;
const userAnswers = new Map(); // {퀴즈 인덱스: 사용자 답변}

// 다음 퀴즈로 이동
function nextQuiz(isAnswered) {
  if(!isAnswered) userAnswers.set(currentQuizIndex, "NONE"); // 사용자 답변 저장

  currentQuizIndex++;
  if (currentQuizIndex < quizzes.length) {
    updateQuiz();
  } else {
    alert("퀴즈가 끝났습니다!");
    endQuiz();
  }
  console.log("다음 퀴즈로 이동하였습니다.");
}
  
// 퀴즈 업데이트
function updateQuiz() {
  const quiz = quizzes[currentQuizIndex];

  try {
    document.getElementById("quiz-question").textContent = quiz.question;
  } catch (error) {
    console.error("에러가 발생했습니다:", error.message);
    console.error("because DOM is not fully loaded yet.");
  }
  const optionsContainer = document.getElementById("quiz-options");
  optionsContainer.innerHTML = ""; // 기존 옵션 삭제
  quiz.options.forEach((option, index) => {
    const button = document.createElement("button");
    button.textContent = `${String.fromCharCode(65 + index)}. ${option}`;
    button.onclick = () => submitAnswer(String.fromCharCode(65 + index));
    optionsContainer.appendChild(button);
  });

  console.log("퀴즈 내용이 업데이트 되었습니다.")
}
  
// 정답 제출
function submitAnswer(selectedOption) {
  const quiz = quizzes[currentQuizIndex];
  userAnswers.set(currentQuizIndex, selectedOption); // 사용자 답변 저장
  if (selectedOption === quiz.answer) {
    alert("정답입니다!");
    score++;
    nextQuiz(1);
  } else {
    alert("틀렸습니다.");
    nextQuiz(1);
  }
}

// 퀴즈 종료
function endQuiz() {
  document.write(`
    <h2>퀴즈 종료!</h2>
    <p>당신의 점수는 <strong>${score} / ${quizzes.length}</strong>입니다.</p>
    <br>
    <div id="user-answers-container"></div>
    <br>
    <button onclick="restartQuiz()">다시 시작</button>
    <br>
    <br>
    <a href="../index.html"> 홈타운으로 돌아가기</a>
  `);

  const container = document.getElementById("user-answers-container");
  let answersHTML = "<h3>사용자 답변</h3><ul>";

  userAnswers.forEach((answer, index) => {
    const quiz = quizzes[index];
    const isCorrect = answer === quiz.answer ? "정답" : "오답";
    const quiz_answer = quiz.options[quiz.answer.charCodeAt(0) - 65];
    const user_answer = (answer !== "NONE") ? quiz.options[answer.charCodeAt(0) - 65] : answer;

    answersHTML += `
      <li>
        <strong>퀴즈 ${index + 1}:</strong> ${quiz.question}<br>
        <strong>정답: </strong> ${quiz_answer} <br>
        <strong>사용자 답변:</strong> ${user_answer}<br>
        <strong>결과:</strong> ${isCorrect}
      </li>
    `;
  });

  container.innerHTML = answersHTML;
}
  
// 퀴즈 다시 시작
function restartQuiz() {
  currentQuizIndex = 0;
  score = 0;
  location.reload(); // 페이지를 새로고침하여 처음 상태로 돌아감
}
  
// 초기화
updateQuiz();
