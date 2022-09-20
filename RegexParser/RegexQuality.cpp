#include "RegexQuality.h"
#include <random>
#include <chrono>

std::wstring GenerateRandomMatchString(const std::shared_ptr<ParseTree>& parseTree)
{
    static std::random_device rd;
    static std::mt19937::result_type seed = rd() ^ (
        (std::mt19937::result_type)
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count() +
        (std::mt19937::result_type)
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count());

    static std::mt19937 gen(seed);
    static std::uniform_int_distribution<unsigned> distrib(0, 100000);

    std::wstring result;

    if (parseTree->content == Regex)
    {
        return GenerateRandomMatchString(parseTree->children[distrib(gen) % parseTree->children.size()]);
    }
    else if (parseTree->content == SimpleRegex)
    {
        for (const auto& child : parseTree->children)
        {
            result += GenerateRandomMatchString(child);
        }
    }
    else
    {
        size_t min, max, reps;

        min = parseTree->repetition.first;
        max = parseTree->repetition.second;
        if (max == -1)
        {
            max = std::max(min, 100ULL);
        }

        reps = distrib(gen) % (max - min + 1) + min;

        if (!parseTree->children.empty())
        {
            for (auto iter = 0; iter < reps; ++iter)
            {
                result += GenerateRandomMatchString(parseTree->children[0]);
            }
        }
        else
        {
            size_t count = parseTree->characterSet.size();
            size_t choice;

            for (auto iter = 0; iter < reps; ++iter)
            {
                choice = distrib(gen) % (count);
                result += parseTree->characterSet.get_nth_element(choice);
            }
        }
    }

    return result;
}

bool VerifySequentialQuantifiers(std::shared_ptr<ParseTree> parseTree)
{
    static const int max_rep_size = 300;
    static const int max_intersection_size = 5;

    bool result = true;

    if (parseTree->content == Regex)
    {
        for (const auto& child : parseTree->children)
        {
            result = result && VerifySequentialQuantifiers(child);
        }
    }
    else if (parseTree->content == SimpleRegex)
    {
        int left_ind = -1, right_ind = -1;
        for (size_t iter = 0; result && iter < parseTree->children.size(); ++iter)
        {
            const auto& child = parseTree->children[iter];
            if (!(child->children.empty()))
            {
                result = result && VerifySequentialQuantifiers(child);
                left_ind = -1;
                right_ind = -1;
            }
            else
            {
                if (child->repetition.second == -1 || child->repetition.second - child->repetition.first >= max_rep_size)
                {
                    left_ind = right_ind;
                    right_ind = iter;

                    if (left_ind != -1)
                    {
                        IntervalSet mutual = child->characterSet;
                        for (int child_iter = left_ind; child_iter < right_ind; ++child_iter)
                        {
                            mutual = mutual.intersection(parseTree->children[child_iter]->characterSet);
                        }

                        if (mutual.size() > max_intersection_size)
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (!parseTree->children.empty())
        {
            result = result && VerifySequentialQuantifiers(parseTree->children[0]);
        }
    }

    return result;
}

std::wstring GenerateAdversarialInput(std::shared_ptr<ParseTree> parseTree)
{
    return L"";
}